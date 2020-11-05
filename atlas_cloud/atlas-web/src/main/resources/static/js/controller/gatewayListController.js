'use strict'

atlas_app.controller('GatewayController',[ '$scope', '$interval', '$uibModal', '$timeout', 'GatewayService', function($scope, $interval, $uibModal, $timeout, GatewayService) {

    $scope.gateways = []; //all the gateways
    $scope.gateway = { alias: '', identity: '', psk: '' }; //the gw from form
    $scope.alertSuccessAdd = false; //Hide|Show success add gateway message
    $scope.alertFailureAdd = false; ////Hide|Show failure add gateway message

    fetchAllGateways();

    /*
    * Get updates of gw data by polling //to do WebSocket
    */
    var fetchAllGatewaysInterval = $interval(function() { fetchAllGateways() }, 2000);

    /*
    * Add a new gateway submit function
    */
    $scope.submit = function() {
        GatewayService.createGateway($scope.gateway)
            .then(
               function (d) {
                  $scope.alertSuccessAdd = true;
                  $scope.alertFailureAdd = false;
                  $timeout(function() {
                       $scope.alertSuccessAdd = false;
                       $scope.reset();
                  }, 2000)
                },
               function (errResponse) {
                  console.error('Error while adding gateway!');
                  $scope.alertSuccessAdd = false;
                  $scope.alertFailureAdd = true;

                  $scope.duplicateValue = errResponse.data.DuplicateValue;
                  $scope.duplicateKey = errResponse.data.DuplicateKey;

                  $timeout(function() {
                      $scope.alertFailureAdd = false;
                   }, 2000)
                }
            );
    };

    /*
    * Reset the form input text boxes
    */
    $scope.reset = function() {
        $scope.gateway = { alias: '', identity: '', psk: '' };
        $scope.duplicateKey = '';
        $scope.searchForm.$setPristine(); //reset Form
    };

    /*
    * Delete gateway
    */

    $scope.remove = function(gw_identity, index){
        var modalInstance = $uibModal.open({
            templateUrl: 'view/modal.html',
            controller: function ($scope, $uibModalInstance, gateways) {
                $scope.ok = function () {
                   gateways.splice(index,1); //delete gateway from selected index
                   GatewayService.deleteGateway(gw_identity);
                   $uibModalInstance.close();
                };
                $scope.cancel = function () {
                   $uibModalInstance.dismiss('cancel');
                };
            },
            resolve: {
                gateways: function() {
                   return $scope.gateways;
                }
            }
        });

        modalInstance.result.then(function(){
            fetchAllGateways();},function(data){});
    };

    /*
    * Fetch the all the gateways using the GatewayService
    */
    function fetchAllGateways() {
        GatewayService.fetchAllGateways()
            .then(
               function (d) {
                    $scope.gateways = d;
               },
               function (errResponse) {
                    console.error('Error while fetching gateways!');
               }
            );
    }

    var expanded_hash_map = {}; //memorize the state of the row

    /*
    * Expand row | Shrink row
    */
     $scope.toggleRow = function (i) {
          expanded_hash_map[i] = !expanded_hash_map[i];
     };

     $scope.isSelected = function (i) {
         return  expanded_hash_map[i];
     };

    /*
    * On destruction event of the controller, cancel the $interval service that makes the polling
    */
    $scope.$on('$destroy', function() {
        if(angular.isDefined(fetchAllGatewaysInterval)) {
             $interval.cancel(fetchAllGatewaysInterval);
        }
    });

}]);
