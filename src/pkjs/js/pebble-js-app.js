var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getData() {
  
  var url = "http://www.hqcasanova.com/co2/all";
  
  xhrRequest(url, 'GET', 
    function(responseText) {
      // scroll through the lines, pull out the numbers,
      // parse the text
      var lines = responseText.split("\n");
      var valCurStr = lines[1].substr(-10,6);
      var val1YrStr = lines[2].substr(-10,6);
      
      var valCur = parseFloat(valCurStr);
      var val1Yr = parseFloat(val1YrStr);
      
      var change1Yr = valCur-val1Yr;
      var change1YrStr = change1Yr.toFixed(2);
      if(change1Yr>=0) {
        change1YrStr = '+' + change1YrStr;
      }
      
      var dictionary = {
        "KEY_READING": valCurStr,
        "KEY_READING_1YR": change1YrStr,
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("Reading info sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending reading info to Pebble!");
        }
      );
    }
  );  
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");

    // Get the initial weather
    getData();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getData();
  }                     
);

Pebble.addEventListener("showConfiguration", function() {
    console.log("showing configuration!");
    Pebble.openURL('http://kunveni-odyssey.s3-website-us-east-1.amazonaws.com/CO2_config.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
  // Decode and parse config data as JSON
  var config_data = JSON.parse(decodeURIComponent(e.response));
  console.log('Config window returned: ', JSON.stringify(config_data));

  // Prepare AppMessage payload
  var dict = {
    "KEY_DATE_MODE": config_data['date-mode']
  };

  // Send settings to Pebble watchapp
  Pebble.sendAppMessage(dict, function(){
    console.log('Sent config data to Pebble');  
  }, function() {
    console.log('Failed to send config data!');
  });
});


