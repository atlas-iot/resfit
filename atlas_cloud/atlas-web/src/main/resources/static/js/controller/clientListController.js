'use strict'

atlas_app.controller('ClientsController',[ '$scope', '$interval', '$route', '$uibModal', '$timeout', 'GatewayService', function($scope, $interval, $route, $uibModal, $timeout, GatewayService) {

    $scope.clients = []; //all the clients of the selected gw
    $scope.gw_identity = $route.current.params.id1; //the selected gw's identity
    $scope.gw_alias = $route.current.params.id2; //he selected gw's alias
    $scope.alertSyncDisplayed = false; //Hide|Show force-sync success message
    $scope.alertSyncFailure = false; //Hide|Show force-sync failure message

    fetchAllClientsSummary($scope.gw_identity);

    /*
    * Get updates of clients data by polling //to do WebSocket
    */
     var fetchAllClientsInterval = $interval(function() {
    	 fetchAllClientsSummary($scope.gw_identity)
         }, 2000);

    /*
    * Delete client
    */
    $scope.remove = function(gw_identity, cl_identity, index){
        var modalInstance = $uibModal.open({
            templateUrl: 'view/modal.html',
            controller: function ($scope, $uibModalInstance, clients) {
              clients.splice(index,1);
              $scope.ok = function () {
                GatewayService.deleteClient(gw_identity, cl_identity);
                $uibModalInstance.close();
              };

              $scope.cancel = function () {
                $uibModalInstance.dismiss('cancel');
              };
            },
            resolve: {
              clients: function() {
                 return $scope.clients;
              }
            }
        });

        modalInstance.result.then(function(){
        	fetchAllClientsSummary($scope.gw_identity);}, function(data){});
    };

    /*
    * Force-Sync gateway clients
    */
    $scope.forceSync = function(gw_identity){
        GatewayService.forceSync(gw_identity)
        .then(
            function () {
               $scope.alertSyncDisplayed = true;
               $timeout(function() {
                  $scope.alertSyncDisplayed = false;
                }, 2000)
            },
            function (errResponse) {
               $scope.alertSyncFailure = true;
               $timeout(function() {
                   $scope.alertSyncFailure = false;
               }, 2000)
            }
         );
    };

    /*
    * Fetch the clients using the GatewayService
    * @param gw_identity selected gw's identity
    */
    function fetchAllClientsSummary(gw_identity){
        GatewayService.fetchAllClientsSummary(gw_identity)
             .then(
                function (d) {
                     $scope.clients = d;
                },
                function (errResponse) {
                     console.error('Error while fetching clients!');
                }
         );
    }

    /*
    * On destruction event of the controller, cancel the $interval service that makes the polling
    */
   $scope.$on('$destroy', function() {
        if(angular.isDefined(fetchAllClientsInterval)) {
             $interval.cancel(fetchAllClientsInterval);
        }
   });
   
   $scope.gotoGatewayList = function() {
	   window.history.back();
   }

}]);