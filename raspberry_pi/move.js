$(document).ready(function() {

	//Time between marker refreshes
	var INTERVAL = 2000;
	
	//Used to remember markers
	var markerStore = {};
	
	var myLatlng = new google.maps.LatLng(-22.979107,-43.233083);
    var myOptions = {
        zoom: 13,
        center: myLatlng,
        mapTypeId: google.maps.MapTypeId.ROADMAP,
    }
    var map = new google.maps.Map(document.getElementById("map_canvas"), myOptions);

	getMarkers();	

	function getMarkers() {
		console.log('getMarkers');
		$.get('/markers', {}, function(res,resp) {
			console.dir(res);
			for(var i=0, len=res.length; i<len; i++) {

				//Do we have this marker already?
				if(markerStore.hasOwnProperty(res[i].id)) {
					console.log('just funna move it...');
					markerStore[res[i].id].setPosition(new google.maps.LatLng(res[i].position.lat,res[i].position.long));
				} else {
					var marker = new google.maps.Marker({
						position: new google.maps.LatLng(res[i].position.lat,res[i].position.long),
						title:res[i].name,
						map:map
					});	
					markerStore[res[i].id] = marker;
					console.log(marker.getTitle());
				}
			}
			window.setTimeout(getMarkers,INTERVAL);
		}, "json");
	}
	
})