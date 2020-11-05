'use strict'

atlas_app.factory('GatewayService', ['$http', '$q', '$location', function ($http, $q, $location) {

    var REST_SERVICE_URI = $location.protocol() + '://' +  $location.host() + ':' + $location.port() + '/atlas/gateways/';
    var REST_SERVICE_URI_GATEWAY = $location.protocol() + '://' +  $location.host() + ':' + $location.port() +  '/atlas/gateway/';

    var factory = {
        fetchAllGateways: fetchAllGateways,
        createGateway: createGateway,
        fetchAllClientsSummary: fetchAllClientsSummary,
        fetchClientDetails: fetchClientDetails,
        deleteGateway: deleteGateway,
        deleteClient: deleteClient,
        forceSync: forceSync,
        updateClientAlias: updateClientAlias,
        sendClientCommand: sendClientCommand
    };

    return factory;

    function fetchAllGateways() {
        var deferred = $q.defer();
        $http.get(REST_SERVICE_URI)
            .then(
                function (response) {
                    deferred.resolve(response.data);
                },
                function (errResponse) {
                    console.error('Error while fetching Gateways');
                    deferred.reject(errResponse);
                }
            );
        return deferred.promise;
    }

    function fetchAllClientsSummary(gw_identity) {
         var deferred = $q.defer();
         $http.get(REST_SERVICE_URI_GATEWAY + 'clients/' + gw_identity)
             .then(
                 function (response) {
                     deferred.resolve(response.data);
                 },
                 function (errResponse) {
                     console.error('Error while fetching clients!');
                     deferred.reject(errResponse);
                 }
             );
         return deferred.promise;
    }

    function fetchClientDetails(gw_identity, client_identity) {
         var deferred = $q.defer();
         $http.get(REST_SERVICE_URI_GATEWAY + 'client/' + gw_identity + '/' + client_identity)
             .then(
                 function (response) {
                     deferred.resolve(response.data);
                 },
                 function (errResponse) {
                     console.error('Error while fetching client details!');
                     deferred.reject(errResponse);
                 }
             );
         return deferred.promise;
    }

    function createGateway(gateway) {
        var deferred = $q.defer();
        $http.post(REST_SERVICE_URI_GATEWAY + 'add', gateway)
            .then(
                function (response) {
                    deferred.resolve(response.data);
                },
                function (errResponse) {
                    console.error('Error while creating Gateway');
                    deferred.reject(errResponse);
                }
            );
        return deferred.promise;
    }

    function deleteGateway(gw_identity) {
        var deferred = $q.defer();
        $http.delete(REST_SERVICE_URI + gw_identity)
            .then(
                function (response) {
                    deferred.resolve(response.data);
                },
                function (errResponse) {
                    console.error('Error while deleting gateway!');
                    deferred.reject(errResponse);
                }
            );
        return deferred.promise;
    }

    function deleteClient(gw_identity, cl_identity) {
        var deferred = $q.defer();
        $http.delete(REST_SERVICE_URI_GATEWAY + 'client/' + gw_identity + '/' + cl_identity)
            .then(
                function (response) {
                    deferred.resolve(response.data);
                },
                function (errResponse) {
                    console.error('Error while deleting client!');
                    deferred.reject(errResponse);
                }
            );
        return deferred.promise;
    }

    function forceSync(gw_identity) {
        var deferred = $q.defer();
        $http.get(REST_SERVICE_URI_GATEWAY + 'force-sync/' + gw_identity)
            .then(
                function (response) {
                    deferred.resolve(response.data);
                },
                function (errResponse) {
                    console.error('Error while force-sync!');
                    deferred.reject(errResponse);
                }
            );
        return deferred.promise;
    }

    function updateClientAlias(gw_identity, cl_identity, alias) {
        var deferred = $q.defer();
        $http.put(REST_SERVICE_URI_GATEWAY + 'client/' + gw_identity + '/' + cl_identity + '/' + alias)
            .then(
                function (response) {
                    deferred.resolve(response.data);
                },
                function (errResponse) {
                    console.error('Error while updating client alias!');
                    deferred.reject(errResponse);
                }
            );
        return deferred.promise;
    }
    
    function sendClientCommand(gw_identity, cl_identity, command) {
    	console.log("Send cmd2222: " + command);
    	var deferred = $q.defer();
        $http.put(REST_SERVICE_URI_GATEWAY + 'client/command/' + gw_identity + '/' + cl_identity + '/' + command)
            .then(
                function (response) {
                	console.log("Send cmd33333: " + command);
                    deferred.resolve(response.data);
                },
                function (errResponse) {
                    console.error('Error while sending command to client!');
                    deferred.reject(errResponse);
                }
            );
        return deferred.promise;
    }

}]);