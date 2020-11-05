'use strict'

atlas_app.controller('ClientDetailsController',[ '$scope', '$interval', '$route', 'GatewayService', function($scope, $interval, $route, GatewayService) {

	const MEMORY_SIZE_1KB = 1024;
	const SYSTEM_LOAD_SCALE = 65536;
    $scope.gw_identity = $route.current.params.id1; //selected gw's psk
    $scope.cl_identity = $route.current.params.id2; //selected client's identity
    $scope.editAliasSelected = false;
    $scope.alias = '';

    $scope.setEditAliasSelected = function() {
        $scope.editAliasSelected = true;
    };

    $scope.aliasFieldChanged = function(alias) {
        GatewayService.updateClientAlias($scope.gw_identity, $scope.cl_identity, alias);

        /* hide ok button*/
        $scope.editAliasSelected = false;
    };
    
    $scope.actionRestart = function(command) {
        GatewayService.sendClientCommand($scope.gw_identity, $scope.cl_identity, 'ATLAS_CMD_CLIENT_DEVICE_RESTART');
    }
    
    $scope.actionShutdown = function(command) {
        GatewayService.sendClientCommand($scope.gw_identity, $scope.cl_identity, 'ATLAS_CMD_CLIENT_DEVICE_SHUTDOWN');
    }

    function convertSecondsToTime(seconds) {
    	var numDays = Math.floor(seconds / 86400);
    	var numHours = Math.floor((seconds % 86400) / 3600);
    	var numMinutes = Math.floor(((seconds % 86400) % 3600) / 60);
    	var numSeconds = ((seconds % 86400) % 3600) % 60;

    	var friendlyTime = "";
    	if (numDays)
    		friendlyTime += numDays + " days ";
    	if (numHours)
    		friendlyTime += numHours + " hours ";
    	if (numMinutes)
    		friendlyTime += numMinutes + " minutes ";
    	if (numSeconds)
    		friendlyTime += numSeconds + " seconds ";
    	
    	return friendlyTime;
    }
    
    fetchClientDetails($scope.gw_identity, $scope.cl_identity);

    /*
    * Get updates of gw data by polling //to do WebSocket
    */
    var fetchClientDetailsInterval = $interval(function() { fetchClientDetails($scope.gw_identity, $scope.cl_identity) }, 2000);

    /*
    * Fetch the client details
    * @param gw_identity selected gw's identity
    * @param cl_identity selected client's identity
    */
    function fetchClientDetails(gw_identity, cl_identity) {
        GatewayService.fetchClientDetails(gw_identity, cl_identity)
             .then(
                function (d) {
                     $scope.client = d;
                     $scope.$broadcast('clientDataChangedEvent', $scope.client);

                     /* Set the initial value of the alias*/
                     if ($scope.alias == '')
                        $scope.alias = $scope.client.alias;

                     /* Scale memory size to KB */
                     if (!isNaN($scope.client.sysinfoBufferram)) {
                    	 $scope.client.sysinfoBufferram /= MEMORY_SIZE_1KB;
                     }
                     if (!isNaN($scope.client.sysinfoFreeram)) {
                    	 $scope.client.sysinfoFreeram /= MEMORY_SIZE_1KB;
                     }
                     if (!isNaN($scope.client.sysinfoTotalswap)) {
                    	 $scope.client.sysinfoTotalswap /= MEMORY_SIZE_1KB;
                     }
                     if (!isNaN($scope.client.sysinfoFreeswap)) {
                    	 $scope.client.sysinfoFreeswap /= MEMORY_SIZE_1KB;
                     }
                     if (!isNaN($scope.client.sysinfoTotalram)) {
                    	 $scope.client.sysinfoTotalram /= MEMORY_SIZE_1KB;
                     }
                     if (!isNaN($scope.client.sysinfoSharedram)) {
                    	 $scope.client.sysinfoSharedram /= MEMORY_SIZE_1KB;
                     }
                     
                     /* Scale system load average */
                     if (!isNaN($scope.client.sysinfoLoad1)) {
                    	 $scope.client.sysinfoLoad1 /= SYSTEM_LOAD_SCALE;
                     }
                     if (!isNaN($scope.client.sysinfoLoad5)) {
                    	 $scope.client.sysinfoLoad5 /= SYSTEM_LOAD_SCALE;
                     }
                     if (!isNaN($scope.client.sysinfoLoad15)) {
                    	 $scope.client.sysinfoLoad15 /= SYSTEM_LOAD_SCALE;
                     }
                     
                     /* Convert uptime from seconds to friendly display time */
                     if (!isNaN($scope.client.sysinfoUptime)) {
                    	 $scope.client.sysinfoUptime = convertSecondsToTime($scope.client.sysinfoUptime);
                     }
                },
                function (errResponse) {
                     console.error('Error while fetching client details!');
                }
         );
    }

   /*
   * On destruction event of the controller, cancel the $interval service that makes the polling
   */
   $scope.$on('$destroy', function() {
        if(angular.isDefined(fetchClientDetailsInterval)) {
             $interval.cancel(fetchClientDetailsInterval);
        }
   });
   
   $scope.gotoClientsList = function() {
	   window.history.back();
   }
}]);