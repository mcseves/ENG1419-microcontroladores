 function initMap() {

      var originalLatLng = new google.maps.LatLng(-22.979107, -43.233083);
      map = new google.maps.Map(document.getElementById('map'), {
        center: originalLatLng, zoom: 8
      }); 
      marker = new google.maps.Marker({
        position: originalLatLng,
        map: map,
        draggable: false
      });

}

function updateMarker() {
  setInterval(function(){

    // aqui as lats e lngs devem ser as referentes as ultimas adicionadas no bd
    var lat = -23.008670
    var lng = -43.369806

    var newlatlng = new google.maps.LatLng(lat,lng);
    marker.setPosition(newlatlng);

  }, 5000);
}

updateMarker();
