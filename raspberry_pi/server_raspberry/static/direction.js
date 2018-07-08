var jsonRoute = {
  "route": []
}

window.initMap = function() {
  var directionsService = new google.maps.DirectionsService();
  var directionsDisplay = new google.maps.DirectionsRenderer();

  var inputStart = document.getElementById('startInput');
  var inputEnd = document.getElementById('endInput');

  var start = inputStart.value;
  var end = inputEnd.value;
  var idObject = document.getElementById('idObjectInput').value;

  var autocompleteStart = new google.maps.places.Autocomplete(inputStart);
  var autocompleteEnd = new google.maps.places.Autocomplete(inputEnd);

  var onPlaceStartChange = function() {

    var placeStart = autocompleteStart.getPlace();
    if (!placeStart.geometry) {
      // User entered the name of a Place that was not suggested and
      // pressed the Enter key, or the Place Details request failed.
      window.alert("No details available for input: '" + placeStart.name + "'");
      return;
    }

    var addressStart = '';
    if (placeStart.address_components) {
      address = [
        (placeStart.address_components[0] && placeStart.address_components[0].short_name || ''),
        (placeStart.address_components[1] && placeStart.address_components[1].short_name || ''),
        (placeStart.address_components[2] && placeStart.address_components[2].short_name || '')
      ].join(' ');
    }
  };

  var onPlaceEndChange = function() {

    var placeEnd = autocompleteEnd.getPlace();
    if (!placeEnd.geometry) {
      // User entered the name of a Place that was not suggested and
      // pressed the Enter key, or the Place Details request failed.
      window.alert("No details available for input: '" + placeStart.name + "'");
      return;
    }

    var addressEnd= '';
    if (placeEnd.address_components) {
      address = [
        (placeEnd.address_components[0] && placeEnd.address_components[0].short_name || ''),
        (placeEnd.address_components[1] && placeEnd.address_components[1].short_name || ''),
        (placeEnd.address_components[2] && placeEnd.address_components[2].short_name || '')
      ].join(' ');
    }
  };

  var onChangeHandler = function() {
      calcRoute(directionsService, directionsDisplay);
    };

  document.getElementById('buttonRoute').addEventListener('click', onChangeHandler);

  autocompleteStart.addListener('place_changed', onPlaceStartChange);
  autocompleteEnd.addListener('place_changed', onPlaceEndChange);
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

      for (var j = 0; j < pointsArray.length; j++)
      {
          var jsonPoints = {};

          jsonPoints["lat"] = pointsArray[j].lat();
          jsonPoints["lng"] = pointsArray[j].lng();
          jsonRoute.route.push(jsonPoints);
      }

      var idObject = document.getElementById('idObjectInput').value;
      ajaxRoute(idObject);

    } else {
      window.alert('Directions request failed due to ' + status);
    }
  });
}

function ajaxRoute(id) {
    $.ajax({
              url:$SCRIPT_ROOT + '/salvar_coord/' + id,
              type: "POST",
              contentType:"application/json",
              dataType:"json",
              data: JSON.stringify(jsonRoute)
    });

    jsonRoute = {"route": []};
}
