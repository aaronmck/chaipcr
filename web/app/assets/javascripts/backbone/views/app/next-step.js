ChaiBioTech.app.Views = ChaiBioTech.app.Views || {}

ChaiBioTech.app.Views.nextStep = Backbone.View.extend({

  className: "next",

  template: JST["backbone/templates/app/previous-step"],

  events : {
    "click .next-img": "selectNext"
  },

  selectNext: function() {
    console.log(ChaiBioTech.app.selectedStep);
  },

  initialize: function() {
    
  },

  render: function() {
    $(this.el).html(this.template());
    return this;
  }
});