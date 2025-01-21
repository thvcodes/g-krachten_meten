const char *s1 = R"=====(<html>
<head>
<script>
	function startRecording() { 
		fetch('/startRecording');
	} 

	function getData() { 
		// replace with /getData
		fetch("/getData.json").then(response => response.json()).then(data => { 
			var pointsX = "";
			var pointsY = "";
			var pointsZ = "";
			var pointsT = "";
      var max = 0;
      data.datapoints.forEach(function(point) {
        point.total = Math.sqrt(point.x*point.x + point.y*point.y + point.z*point.z);
        max = Math.max(max, Math.abs(point.x));
        max = Math.max(max, Math.abs(point.y));
        max = Math.max(max, Math.abs(point.z));
        max = Math.max(max, point.total);
      });
      const scale = Math.ceil(max > 0 ? 200 / max : 0);
      const maxLine = Math.ceil(scale > 0 ? 200 / scale : max);

      var y0 = document.getElementById("y0"); y0.textContent = maxLine.toString();
      var y1 = document.getElementById("y1"); y1.textContent = (maxLine/2).toString();
      var y2 = document.getElementById("y2"); y2.textContent = (-maxLine/2).toString();
      var y3 = document.getElementById("y3"); y3.textContent = (-maxLine).toString();

      var avgX= 0; var maxX = 0;
      var avgY= 0; var maxY = 0; 
      var avgZ= 0; var maxZ = 0;
      var avgTotal = 0; var maxTotal = 0;
      var activePoints = 0;
      data.datapoints.forEach(function(point) {        
        // max
        if (Math.abs(point.x) > maxX) maxX = Math.abs(point.x);
        if (Math.abs(point.y) > maxY) maxY = Math.abs(point.y);
        if (Math.abs(point.z) > maxZ) maxZ = Math.abs(point.z);
        if (point.total > maxTotal) maxTotal = point.total;

        // avg when moving
        if(point.total > 1.2) {
          avgX += Math.abs(point.x);
          avgY += Math.abs(point.y);
          avgZ += Math.abs(point.z);
          avgTotal += point.total;
          activePoints++;
        }

        pointsX += `${point.millis/20+100}, ${200 - scale * point.x} `;
        pointsY += `${point.millis/20+100}, ${200 - scale * point.y} `;
        pointsZ += `${point.millis/20+100}, ${200 - scale * point.z} `;
        pointsT += `${point.millis/20+100}, ${200 - scale * point.total} `;
      });
      var plineX = document.getElementById("plineX"); plineX.setAttribute("points", pointsX);
      var plineY = document.getElementById("plineY"); plineY.setAttribute("points", pointsY);
      var plineZ = document.getElementById("plineZ"); plineZ.setAttribute("points", pointsZ);
      var plineT = document.getElementById("plineT"); plineT.setAttribute("points", pointsT);

      document.getElementById('avgX').innerText = (avgX/activePoints).toFixed(2) + ' g'; 
      document.getElementById('avgY').innerText = (avgY/activePoints).toFixed(2) + ' g'; 
      document.getElementById('avgZ').innerText = (avgZ/activePoints).toFixed(2) + ' g'; 
      document.getElementById('avgT').innerText = (avgTotal/activePoints).toFixed(2) + ' g'; 
      document.getElementById('maxX').innerText = maxX.toFixed(2) + ' g'; 
      document.getElementById('maxY').innerText = maxY.toFixed(2) + ' g'; 
      document.getElementById('maxZ').innerText = maxZ.toFixed(2) + ' g'; 
      document.getElementById('maxT').innerText = maxTotal.toFixed(2) + ' g';
		}); 
	} 
</script> 
<style>
body {
  font-family: 'Open Sans', sans-serif;
}

.graph .labels.x-labels {
  text-anchor: middle;
}

.graph .labels.y-labels {
  text-anchor: end;
}

.graph {
  margin-left: 10%;

  height: 500px;
  width: 800px;
}

.graph .grid {
  stroke: #ccc;
  stroke-dasharray: 0;
  stroke-width: 1;
}

.labels {
  font-size: 13px;
}

.label-title {
  font-weight: bold;
  text-transform: uppercase;
  font-size: 12px;
  fill: black;
}

.data {
  fill: red;
  stroke-width: 1; 
}
.data1 {
  fill: green;
  stroke-width: 1; 
}

.chart_circle1 {
    stroke: #0074d9;
    stroke-width: 1px;
    opacity: 0;
}

.chart_circle1:hover {
    opacity: 1;
}

.chart_data_1 {
  stroke: #0074D9;
}
circle.chart_data_1 {
  fill: #0074D9;
}
.chart_data_2 {
    stroke: #FF851B;
}
circle.chart_data_2 {
  fill: #FF851B;
}
.chart_data_3 {
    stroke: #2ECC40;
}
circle.chart_data_3 {
  fill: #2ECC40;
}
.chart_data_4 {
    stroke: #B10DC9;
}
circle.chart_data_4 {
  fill: #B10DC9;
}

h1 { 
  text-align: center;
}
table {
  margin-left: 10%;
  border-collapse: collapse; /* Ensures borders do not double up */
  width: 80%; /* Optional, for full-width table */
}
table, th, td {
  border: 1px solid black; /* Uniform border for table and cells */
}
th, td {
  padding: 8px; /* Adds spacing inside cells */
  text-align: left; /* Aligns text to the left */
}

</style>

</head>
<body>

  <h1>Miniatuur achtbaan</h1><br/>

<svg version="1.2" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" class="graph" aria-labelledby="title" role="img">
  <title id="title">Overzicht G krachten over de tijd</title>
<g class="grid x-grid" id="xGrid">
  <line x1="100" x2="100" y1="0" y2="400"></line>
</g>
<g class="grid y-grid" id="yGrid">
  <line x1="100" x2="600" y1="0" y2="0"></line>
  <line x1="100" x2="600" y1="100" y2="100"></line>
  <line x1="100" x2="600" y1="200" y2="200"></line>
  <line x1="100" x2="600" y1="300" y2="300"></line>
  <line x1="100" x2="600" y1="400" y2="400"></line>
</g>
<g class="labels x-labels">
  <text x="100" y="420">0</text>
  <text x="150" y="420">1</text>
  <text x="200" y="420">2</text>
  <text x="250" y="420">3</text>
  <text x="300" y="420">4</text>
  <text x="350" y="420">5</text>
  <text x="400" y="420">6</text>
  <text x="450" y="420">7</text>
  <text x="500" y="420">8</text>
  <text x="550" y="420">9</text>
  <text x="600" y="420">10</text>
  <text x="400" y="440" class="label-title">Tijd in seconden</text>
</g>
<g class="labels line-labels">
  <circle cx="100" cy="455" data-value="X" r="10" class="chart_data_1"></circle>
  <text x="115" y="460">X</text>
  <circle cx="150" cy="455" data-value="X" r="10" class="chart_data_2"></circle>
  <text x="165" y="460">Y</text>
  <circle cx="200" cy="455" data-value="X" r="10" class="chart_data_3"></circle>
  <text x="215" y="460">Z</text>
  <circle cx="250" cy="455" data-value="X" r="10" class="chart_data_4"></circle>
  <text x="265" y="460">Totaal</text>
</g>
<g class="labels y-labels">
  <text x="80" y="10" id="y0">4</text>
  <text x="80" y="110" id="y1">2</text>
  <text x="80" y="210">0</text>
  <text x="80" y="310" id="y2">-2</text>
  <text x="80" y="400" id="y3">-4</text>
  <text x="80" y="160" class="label-title">G kracht</text>
</g>
<!-- <g class="data" data-setname="Our first data set">
  <circle cx="90" cy="192" data-value="7.2" r="4"></circle>
  <circle cx="240" cy="141" data-value="8.1" r="4"></circle>
  <circle cx="388" cy="179" data-value="7.7" r="4"></circle>
  <circle cx="531" cy="200" data-value="6.8" r="4"></circle>
  <circle cx="677" cy="104" data-value="6.7" r="4"></circle>
</g> -->

<polyline id="plineX" fill="none" class="chart_data_1" stroke-width="2" points="" />
<polyline id="plineY" fill="none" class="chart_data_2" stroke-width="2" points="" />
<polyline id="plineZ" fill="none" class="chart_data_3" stroke-width="2" points="" />
<polyline id="plineT" fill="none" class="chart_data_4" stroke-width="2" points="" />

<g>
	<circle class="chart_circle1" cx="120" cy="60" r="4" />
	<title>60</title>
</g>

</svg>

<div style="margin-left: 10%;">
  <button onclick="startRecording();">Start Recording</button> &nbsp; &nbsp; <button onclick="getData();">Get Data</button>
</div>
<br /><br />

<table>
  <tr>
    <th>G kracht</th>
    <th>X</th>
    <th>Y</th>
    <th>Z</th>
    <th>Totaal</th>
  </tr>
  <tr>
    <td>Gemiddeld</td>
    <td><span id="avgX">-</span></td>
    <td><span id="avgY">-</span></td>
    <td><span id="avgZ">-</span></td>
    <td><span id="avgT">-</span></td>
  </tr>
  <tr>
    <td>Maximaal</td>
    <td><span id="maxX">-</span></td>
    <td><span id="maxY">-</span></td>
    <td><span id="maxZ">-</span></td>
    <td><span id="maxT">-</span></td>
  </tr>
</table>

<br/><br/>
<!-- 
https://css-tricks.com/how-to-make-charts-with-svg/
https://claudiorimann.com/svg-charts-without-javascript-part-1/
https://www.w3schools.com/graphics/svg_path.asp
 -->

</body>
</html>)=====";