window.ChaiBioTech.ngApp.factory('circleManager', [
  'path',
  function(path) {

    this.init = function(kanvas) {
      
      this.originalCanvas = kanvas;
      this.allStepViews = kanvas.allStepViews;
      this.allCircles = kanvas.allCircles;
      this.findAllCirclesArray = kanvas.findAllCirclesArray;
      this.drawCirclesArray = kanvas.drawCirclesArray;
      this.canvas = kanvas.canvas;
    };

    this.togglePaths = function(toggle) {

      this.allCircles.forEach(function(circle, index) {
        if(circle.curve) {
          circle.curve.setVisible(toggle);
        }
      }, this);

    };

    this.addRampLinesAndCircles = function(circles) {

      this.originalCanvas.allCircles = this.allCircles = circles || this.findAllCircles();
      var limit = this.allCircles.length;

      this.allCircles.forEach(function(circle, index) {

        if(index < (limit - 1)) {
          circle.moveCircle();
          circle.curve = new path(circle);
          this.canvas.add(circle.curve);
        }

        circle.getCircle();
        this.canvas.bringToFront(circle.parent.rampSpeedGroup);
      }, this);

      // We should put an infinity symbol if the last step has infinite hold time.
      this.allCircles[limit - 1].doThingsForLast();
      console.log("All circles are added ....!!");
      return this;
    };

    this.findAllCircles = function() {

      var tempCirc = null;
      this.findAllCirclesArray.length = 0;

      this.findAllCirclesArray = this.allStepViews.map(function(step) {

        if(tempCirc) {
          step.circle.previous = tempCirc;
          tempCirc.next = step.circle;
        }
        tempCirc = step.circle;
        return step.circle;
      });

      return this.findAllCirclesArray;
    };

    this.reDrawCircles = function() {

      var tempCirc = null;
      this.drawCirclesArray.length = 0;

      this.drawCirclesArray = this.allStepViews.map(function(step, index) {

        step.circle.removeContents();
        delete step.circle;
        step.addCircle();

        if(tempCirc) {
          step.circle.previous = tempCirc;
          tempCirc.next = step.circle;
        }

        tempCirc = step.circle;
        return step.circle;
      }, this);

      return this.drawCirclesArray;
    };

    return this;
  }
]);