const char *s1 = R"=====(<html>
<head>
<script>
	function startRecording() { 
    setCircle(1, 'off'); setCircle(2, 'off'); setCircle(3, 'off'); setCircle(4, 'off');

    fetch('/startRecording').then(response => response.text()).then(data => {
      document.getElementById('status').innerText = data;
      setTimeout(getData, 9000);
    });
	}

	function getData() { 
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

        // avg als beweegt
        if(point.total > 1.2) {
          avgX += Math.abs(point.x);
          avgY += Math.abs(point.y);
          avgZ += Math.abs(point.z);
          avgTotal += point.total;
          activePoints++;
        }

        pointsX += `${point.millis/10+100}, ${200 - scale * point.x} `;
        pointsY += `${point.millis/10+100}, ${200 - scale * point.y} `;
        pointsZ += `${point.millis/10+100}, ${200 - scale * point.z} `;
        pointsT += `${point.millis/10+100}, ${200 - scale * point.total} `;
      });
      var plineX = document.getElementById("pline1"); plineX.setAttribute("points", pointsX); setCircle(1, 'on');
      var plineY = document.getElementById("pline2"); plineY.setAttribute("points", pointsY); setCircle(2, 'on');
      var plineZ = document.getElementById("pline3"); plineZ.setAttribute("points", pointsZ); setCircle(3, 'on');
      var plineT = document.getElementById("pline4"); plineT.setAttribute("points", pointsT); setCircle(4, 'on');

      if(activePoints == 0) {
        activePoints = 1; 
      }
      document.getElementById('avgX').innerText = (avgX/activePoints).toFixed(2) + ' g'; 
      document.getElementById('avgY').innerText = (avgY/activePoints).toFixed(2) + ' g'; 
      document.getElementById('avgZ').innerText = (avgZ/activePoints).toFixed(2) + ' g'; 
      document.getElementById('avgT').innerText = (avgTotal/activePoints).toFixed(2) + ' g'; 
      document.getElementById('maxX').innerText = maxX.toFixed(2) + ' g'; 
      document.getElementById('maxY').innerText = maxY.toFixed(2) + ' g'; 
      document.getElementById('maxZ').innerText = maxZ.toFixed(2) + ' g'; 
      document.getElementById('maxT').innerText = maxTotal.toFixed(2) + ' g';

      document.getElementById('status').innerText = "Klaar";
		});
  }
 
  function setCircle(circleId, state) {
    var circle = document.getElementById(`circle${circleId}`);
    var pline = document.getElementById(`pline${circleId}`);
    if(state === 'on') {
      circle.classList.add(`chart_data_${circleId}_filled`);
      pline.style.display = "";
    } else {
      circle.classList.remove(`chart_data_${circleId}_filled`);
      pline.style.display = "none";
    }
  }

  function circleClicked(circleId) {
    var circle = document.getElementById(`circle${circleId}`);
    if(circle.classList.contains(`chart_data_${circleId}_filled`)) {
      setCircle(circleId, 'off');
    } else {
      setCircle(circleId, 'on');
    }
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
  display: block; 
  margin: auto;
  height: 500px;
  width: 950px;
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

.chart_data_1 {
  stroke: #0074D9;
  fill: none;
}
circle.chart_data_1_filled {
  fill: #0074D9;
}

.chart_data_2 {
    stroke: #FF851B;
    fill: none;
}
circle.chart_data_2_filled {
  fill: #FF851B;
}

.chart_data_3 {
    stroke: #2ECC40;
    fill: none;
}
circle.chart_data_3_filled {
  fill: #2ECC40;
}

.chart_data_4 {
  stroke: #B10DC9;
  fill: none;
}
circle.chart_data_4_filled {
  fill: #B10DC9;
}

h1 { 
  text-align: center;
}

span.status {
  font-size: x-large;
}

button {
  height: 50px;
  width: 250px;
  font-size: large;
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
      <line x1="100" x2="900" y1="0" y2="0"></line>
      <line x1="100" x2="900" y1="100" y2="100"></line>
      <line x1="100" x2="900" y1="200" y2="200"></line>
      <line x1="100" x2="900" y1="300" y2="300"></line>
      <line x1="100" x2="900" y1="400" y2="400"></line>
    </g>
    <g class="labels x-labels">
      <text x="100" y="420">0</text>
      <text x="200" y="420">1</text>
      <text x="300" y="420">2</text>
      <text x="400" y="420">3</text>
      <text x="500" y="420">4</text>
      <text x="600" y="420">5</text>
      <text x="700" y="420">6</text>
      <text x="800" y="420">7</text>
      <text x="900" y="420">8</text>
      <text x="400" y="440" class="label-title">Tijd in seconden</text>
    </g>
    <g class="labels line-labels">
      <circle id="circle1" cx="100" cy="455" data-value="X" r="10" class="chart_data_1" pointer-events="visible" onclick="circleClicked(1)"></circle>
      <text id="text1" x="115" y="460" onclick="circleClicked(1)">X</text>
      <circle id="circle2" cx="150" cy="455" data-value="X" r="10" class="chart_data_2" pointer-events="visible" onclick="circleClicked(2)"></circle>
      <text id="text2" x="165" y="460" onclick="circleClicked(2)">Y</text>
      <circle id="circle3" cx="200" cy="455" data-value="X" r="10" class="chart_data_3" pointer-events="visible" onclick="circleClicked(3)"></circle>
      <text id="text3" x="215" y="460" onclick="circleClicked(3)">Z</text>
      <circle id="circle4" cx="250" cy="455" data-value="X" r="10" class="chart_data_4" pointer-events="visible" onclick="circleClicked(4)"></circle>
      <text id="text4" x="265" y="460" onclick="circleClicked(4)">Totaal</text>
    </g>
    <g class="labels y-labels">
      <text x="80" y="10" id="y0">4</text>
      <text x="80" y="110" id="y1">2</text>
      <text x="80" y="210">0</text>
      <text x="80" y="310" id="y2">-2</text>
      <text x="80" y="400" id="y3">-4</text>
      <text x="80" y="160" class="label-title">G kracht</text>
    </g>

    <polyline id="pline1" fill="none" class="chart_data_1" stroke-width="2" points="" />
    <polyline id="pline2" fill="none" class="chart_data_2" stroke-width="2" points="" />
    <polyline id="pline3" fill="none" class="chart_data_3" stroke-width="2" points="" />
    <polyline id="pline4" fill="none" class="chart_data_4" stroke-width="2" points="" />
  </svg>

  <div style="margin-left: 10%;">
    <button onclick="startRecording();">Start Recording</button> &nbsp; &nbsp; &nbsp; &nbsp;
    <span class="status"> Status: </span><span class="status" id="status">klaar</span>
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