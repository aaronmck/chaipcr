ChaiBioTech.app.Views = ChaiBioTech.app.Views || {};

ChaiBioTech.app.Views.fabricEvents = function(C) {
  // C is canvas object and C.canvas is the fabric canvas object
  this.canvas = C.canvas;

  this.canvas.on("mouse:down", function(evt) {
    if(evt.target) {
      switch(evt.target.name)  {

      case "step":
        var me = evt.target.me;
        me.circle.manageClick();
        // Sending data to backbone
        appRouter.editStageStep.trigger("stepSelected", me);
      break;

      case "controlCircleGroup":
        var me = evt.target.me;
        me.manageClick();
        appRouter.editStageStep.trigger("stepSelected", me.parent);
      break;
      }
    }
  });
  // For dragging
  this.canvas.on('object:moving', function(evt) {
    if(evt.target) {
      switch(evt.target.name) {
        case "controlCircleGroup":
          var targetCircleGroup = evt.target,
          me = evt.target.me;
          me.manageDrag(targetCircleGroup);
          appRouter.editStageStep.trigger("stepDrag", me);
        break;
      }
    }
  });
  // when scrolling is finished
  this.canvas.on('object:modified', function(evt) {
    if(evt.target) {
      if(evt.target.name === "controlCircleGroup") {// Right now we have only one item here otherwise switch case
        var me = evt.target.me;
        var targetCircleGroup = evt.target;
        var temp;
        appRouter.editStageStep.trigger("stepDrag", me);
        temp = evt.target.me.temperature.text;
        me.model.changeTemperature(parseFloat(temp.substr(0, temp.length - 1)));
      }
    }
  });
  // We add this handler so that canvas works when scrolled
  $(".canvas-containing").scroll(function(){
    C.canvas.calcOffset();
  });

  // When all the images are loaded up
  // We fire this event
  // Note that it takes some more time to load images, better avaoid images
  // or wait for images to complete

  this.canvas.on("imagesLoaded", function() {
    C.addRampLinesAndCircles();
    C.selectStep();
    C.canvas.renderAll();
  });

 // Changed from bottom means , those values were changed from bottom
 // of the screen where we can type in those values

 this.canvas.on("temperatureChangedFromBottom", function(changedStep) {
    changedStep.circle.getTop();
    changedStep.circle.circleGroup.top = changedStep.circle.top;
    changedStep.circle.manageDrag(changedStep.circle.circleGroup);
    changedStep.circle.circleGroup.setCoords();
  });

  this.canvas.on("rampSpeedChangedFromBottom", function(changedStep) {
    changedStep.showHideRamp();
  });

  this.canvas.on("stepNameChangedFromBottom", function(changedStep) {
    changedStep.updateStepName();
  });

  this.canvas.on("cycleChangedFromBottom", function(changedStep) {
    changedStep.parentStage.changeCycle();
  });

  // When a model in the server changed
  // changes like add step/stage or delete step/stage

  this.canvas.on("modelChanged", function(evt) {
    C.model.getLatestModel(C.canvas);
    C.canvas.clear();
  });

  this.canvas.on("latestData", function() {
    while(C.allStepViews.length > 0) {
      C.allStepViews.pop();
    }
    ChaiBioTech.app.selectedStage = null;
    ChaiBioTech.app.selectedStep = null;
    ChaiBioTech.app.selectedCircle = null;
    C.addStages().setDefaultWidthHeight().addinvisibleFooterToStep();
  });
}