ChaiBioTech.Data =  ChaiBioTech.Data || {};
ChaiBioTech.Constants =  ChaiBioTech.Constants || {};

var originalStepHeight = 200, tempBarHeight = 18;  //actual height is 18
ChaiBioTech.Constants = {
	"stepHeight": originalStepHeight - tempBarHeight,
	"stepUnitMovement": (originalStepHeight - tempBarHeight) /100, //No more used
	"stepWidth": 150,
	"tempBarWidth": 45,
	"tempBarHeight": tempBarHeight,
	"beginningTemp": 25,
	"originalStepHeight": originalStepHeight,
	"rad2deg": 180/Math.PI
}