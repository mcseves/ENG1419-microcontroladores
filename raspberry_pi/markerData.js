//initialize markerdata
var markerData = [];
markerData.push({id:1,name:"Truck", position:{"lat":-22.989107,"long":-43.243083}});

function moveMarkers() {
	for(var i=0, len=markerData.length; i<len; i++) {
		var thisMarker = markerData[i];
		//adjust both lat and long a bit, unless we don't move
		if(shouldMove()) {
			//0.002 seems like a nice amount
			thisMarker.position.lat += randRange(-0.002,0.002);
			thisMarker.position.long += randRange(-0.002,0.002);
		}
	}
	
}

function shouldMove() {
	return Math.random() > 0.2;	
}

//credit: http://stackoverflow.com/a/1527820/52160
function randRange(min, max) {
    return Math.random() * (max - min) + min;
}

exports.getMarkers = function() {
	moveMarkers();
	
	//return
	return markerData;
}