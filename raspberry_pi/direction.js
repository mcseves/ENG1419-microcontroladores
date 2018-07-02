var jsonRoute = {
  "route": []
}

function initMap() {
  var directionsService = new google.maps.DirectionsService();
  var directionsDisplay = new google.maps.DirectionsRenderer();
  var chicago = new google.maps.LatLng(41.850033, -87.6500523);
  var mapOptions = {
    zoom:7,
    center: chicago
  }

  var onChangeHandler = function() {
      calcRoute(directionsService, directionsDisplay);
    };

    document.getElementById('buttonRoute').addEventListener('click', onChangeHandler);
}

function calcRoute(directionsService, directionsDisplay) {
  var start = document.getElementById('startInput').value;
  var end = document.getElementById('endInput').value;
  var request = {
    origin: start,
    destination: end,
    travelMode: 'DRIVING'
  };
  directionsService.route(request, function(result, status) {
    if (status == 'OK') {
      directionsDisplay.setDirections(result);

      var pointsArray = result.routes[0].overview_path;

      var jsonPoints = {};      

      for (var j = 0; j < pointsArray.length; j++)
      {
          jsonPoints["lat"] = pointsArray[j].lat();
          jsonPoints["lng"] = pointsArray[j].lng();
          jsonRoute.route.push(jsonPoints);

      }
 
    } else {
      window.alert('Directions request failed due to ' + status);
    }
  });
}
