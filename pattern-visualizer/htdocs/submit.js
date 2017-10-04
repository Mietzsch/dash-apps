function submitParams(e) {
  e.preventDefault();
  sendPOST();
}

function sendPOST() {
  var req = new XMLHttpRequest();
  req.open("POST","pattern.php");
  req.addEventListener("load", function(e) {
    var responseText = e.target.responseText;
    console.log(responseText)
    var response = JSON.parse(responseText.slice(0,responseText.indexOf("}")+1));
    // TODO rewrite when server sends json instead of svg
    if(response.success) {
      var svgText = responseText.slice(responseText.indexOf("}")+1,-1);
      var parser = new DOMParser();
      var result = parser.parseFromString(svgText,"image/svg+xml");
      var exportLink = document.createElement("a");
      exportLink.setAttribute("href","data:image/svg+xml;base64,"+window.btoa(svgText));
      exportLink.setAttribute("download",response.name);
      exportLink.setAttribute("target","_blank");
      exportLink.appendChild(document.createTextNode("Export as svg"));
      var imageDiv = document.createElement("div");
      imageDiv.appendChild(result.documentElement);
      displayResult([exportLink,imageDiv]);
    } else {
      displayResult([document.createTextNode(response.error)]);
    }
  });
  req.addEventListener("error", function(e) {
    var errorMsg = "Error loading visualized pattern, try again";
    console.warn(e);
    console.error(errorMsg);
    displayResult([document.createTextNode(errorMsg)]);
  });
  req.send(JSON.stringify(getParams()));
}

function displayResult(result) {
  var cont = document.getElementById("result");
  // clear previous result
  while(cont.firstChild) {
    cont.removeChild(cont.firstChild);
  }

  for(var i=0; i < result.length; i++) {
    cont.appendChild(result[i]);
  }
}

