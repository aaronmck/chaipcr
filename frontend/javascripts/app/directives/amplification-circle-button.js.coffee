###
Chai PCR - Software platform for Open qPCR and Chai's Real-Time PCR instruments.
For more information visit http://www.chaibio.com

Copyright 2016 Chai Biotechnologies Inc. <info@chaibio.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
###
window.ChaiBioTech.ngApp
.directive 'amplificationCircleButton', [
  'Device'
  (Device) ->
    restrict: 'EA'
    require: 'ngModel'
    replace: true
    templateUrl: 'app/views/directives/amplification-circle-button.html'
    link: ($scope, elem, attrs, ngModel) ->

      Device.isDualChannel().then (is_dual_channel) ->
        $scope.is_dual_channel = is_dual_channel

      $scope.$watchCollection ->
        ngModel.$modelValue
      , (newVal) ->
        $scope.updateUI() if newVal

      $scope.updateUI = ->
        $scope.selected = ngModel.$modelValue.selected
        $scope.color = ngModel.$modelValue.color || 'gray'
        $scope.ct = ngModel.$modelValue.ct

        $scope.style =
          borderColor: $scope.color
          paddingLeft: if (!ngModel.$modelValue.ct?[0] and !ngModel.$modelValue.ct?[1]) then '0px' else '10px'

      $scope.toggleState = ->
        state =
          selected: !ngModel.$modelValue.selected || false
          color: ngModel.$modelValue.color || 'gray'
          ct: $scope.ct

        ngModel.$setViewValue state
        $scope.updateUI()

]