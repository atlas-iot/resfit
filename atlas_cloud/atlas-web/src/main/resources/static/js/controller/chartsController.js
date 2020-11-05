'use strict'

atlas_app.controller('ChartsController',[ '$scope', '$filter', function($scope, $filter) {

	/* Maximum number of samples in the plot (5 minute sample rate) */
	const PLOT_MAX_SAMPLES = 288
    
	$scope.client = $scope.$parent.client;
    $scope.firewallValues = [];
    $scope.reputationValues = [];
    
    /* Last plot samples dates */
    $scope.lastSampleDate = {
    	firewallIngress: '',
    	firewallEgress: '',
    	reputation: ''
    };

    $scope.$on('clientDataChangedEvent', function (events, data) {
    	$scope.client = data;

        /* System reputation samples */
        if ($scope.client.reputationHistory.length > 0) {
        	var currentRepDate = $scope.client.reputationHistory[$scope.client.reputationHistory.length - 1].date;
        	/* If the last value is updated, then we must update the plot */
        	if (currentRepDate != $scope.lastSampleDate.reputation) {
        		$scope.systemReputationHistory = $scope.client.reputationHistory.map(x => x.system);
        		$scope.temperatureReputationHistory = $scope.client.reputationHistory.map(x => x.temperature);
        		var dates = $scope.client.reputationHistory.map(x => x.date);
        		$scope.reputationTimeLabels = [];
        		angular.forEach(dates, function(value, key) {
                	$scope.reputationTimeLabels.push($filter('date')(value,'yyyy-MM-dd <br> HH:mm:ss'));
        		})
        		$scope.lastSampleDate.reputation = currentRepDate;
            	/* Update plot */
        		$scope.reputationSelectedIntervalChanged();
        	}
        }

        /* Firewall ingress */
        if ($scope.client.ingressFirewallHistory.length > 0) {
        	var currentFwIngressDate = $scope.client.ingressFirewallHistory[$scope.client.ingressFirewallHistory.length - 1].date;
        	/* If the last value is updated, then we must update the plot */
        	if (currentFwIngressDate != $scope.lastSampleDate.firewallIngress) {
        		$scope.ingressFwPassedHistory = $scope.client.ingressFirewallHistory.map(x => x.passed);
        		$scope.ingressFwDroppedHistory = $scope.client.ingressFirewallHistory.map(x => x.dropped);
        		var dates = $scope.client.ingressFirewallHistory.map(x => x.date);
        		$scope.fwIngressTimeLabels = [];
        		angular.forEach(dates, function(value, key) {
                	$scope.fwIngressTimeLabels.push($filter('date')(value,'yyyy-MM-dd <br> HH:mm:ss'));
        		})
        		$scope.lastSampleDate.firewallIngress = currentFwIngressDate;
        		/* Update ingress plot only if ingress is selected */
        		if($scope.firewallSelectedDirection != 'egress') {
        			$scope.firewallIngressUpdatePlot();
        		}
        	}
        }
        
        /* Firewall egress */
        if ($scope.client.egressFirewallHistory.length > 0) {
        	var currentFwEgressDate = $scope.client.egressFirewallHistory[$scope.client.egressFirewallHistory.length - 1].date;
        	/* If the last value is updated, then we must update the plot */
        	if (currentFwEgressDate != $scope.lastSampleDate.firewallEgress) {
        		$scope.egressFwPassedHistory = $scope.client.egressFirewallHistory.map(x => x.passed);
        		$scope.egressFwDroppedHistory = $scope.client.egressFirewallHistory.map(x => x.dropped);
        		var dates = $scope.client.egressFirewallHistory.map(x => x.date);
        		$scope.fwEgressTimeLabels = [];
        		angular.forEach(dates, function(value, key) {
                	$scope.fwEgressTimeLabels.push($filter('date')(value,'yyyy-MM-dd <br> HH:mm:ss'));
        		})
         		$scope.lastSampleDate.firewallEgress = currentFwEgressDate;
        		/* Update egress plot only if egress is selected */
        		if($scope.firewallSelectedDirection == 'egress') {
        			$scope.firewallEgressUpdatePlot();
        		}
        	}
        }
    });

    /* Selected option for time interval (12h or 24h) */
    $scope.firewallSelectedInterval = '';
    $scope.reputationSelectedInterval = '';

    /* Ingress or egress data */
    $scope.firewallSelectedDirection = '';

    /*
    * Callback function when selected interval for firewall graph
    * is chanced or selected direction for firewall changed
    */
    $scope.firewallEgressUpdatePlot = function() {
        /* Update time labels for x-axis */
        $scope.firewallTimeLabelsUpdate($scope.fwEgressTimeLabels);

    	if($scope.firewallSelectedInterval == 'last_day') {
            /* Get PLOT_MAX_SAMPLES/2 samples from even positions */
            var filteredEgressFwPassedHistory = $scope.egressFwPassedHistory.filter((a,i) => i % 2 === 0);
            var filteredEgressFwDroppedHistory = $scope.egressFwDroppedHistory.filter((a,i) => i % 2 === 0);

            $scope.firewallValues[0] = filteredEgressFwPassedHistory;
            $scope.firewallValues[1] = filteredEgressFwDroppedHistory;
        } else {
            /* Last 12h (last PLOT_MAX_SAMPLES/2 values) */
            if($scope.egressFwPassedHistory.length < PLOT_MAX_SAMPLES / 2)
                $scope.firewallValues[0] = $scope.egressFwPassedHistory;
            else
            	$scope.firewallValues[0] = $scope.egressFwPassedHistory.slice($scope.egressFwPassedHistory.length - PLOT_MAX_SAMPLES / 2,
            																  $scope.egressFwPassedHistory.length);
            
            if($scope.egressFwDroppedHistory.length < PLOT_MAX_SAMPLES / 2)
                $scope.firewallValues[1] = $scope.egressFwDroppedHistory;
            else
            	$scope.firewallValues[1] = $scope.egressFwDroppedHistory.slice($scope.egressFwDroppedHistory.length - PLOT_MAX_SAMPLES / 2,
            																   $scope.egressFwDroppedHistory.length);
        }
    }
    
    $scope.firewallIngressUpdatePlot = function() {
        /* Update time labels for x-axis */
        $scope.firewallTimeLabelsUpdate($scope.fwIngressTimeLabels);

        if($scope.firewallSelectedInterval == 'last_day') {
            /* Get PLOT_MAX_SAMPLES/2 samples from even positions */
            var filteredIngressFwPassedHistory = $scope.ingressFwPassedHistory.filter((a,i) => i % 2 === 0);
            var filteredIngressFwDroppedHistory = $scope.ingressFwDroppedHistory.filter((a,i) => i % 2 === 0);

            $scope.firewallValues[0] = filteredIngressFwPassedHistory;
            $scope.firewallValues[1] = filteredIngressFwDroppedHistory;
        } else {
        	/* Last 12h (last PLOT_MAX_SAMPLES/2 values) */
            if($scope.ingressFwPassedHistory.length < PLOT_MAX_SAMPLES / 2)
                $scope.firewallValues[0] = $scope.ingressFwPassedHistory;
            else
            	$scope.firewallValues[0] = $scope.ingressFwPassedHistory.slice($scope.ingressFwPassedHistory.length - PLOT_MAX_SAMPLES / 2,
            															  	   $scope.ingressFwPassedHistory.length);
            
            if($scope.ingressFwDroppedHistory.length < PLOT_MAX_SAMPLES / 2)
                $scope.firewallValues[1] = $scope.ingressFwDroppedHistory;
            else
            	$scope.firewallValues[1] = $scope.ingressFwDroppedHistory.slice($scope.ingressFwDroppedHistory.length - PLOT_MAX_SAMPLES / 2,
            																	$scope.ingressFwDroppedHistory.length);
        }
    };

    $scope.firewallTimeLabelsUpdate = function(timeLabels) {
        if($scope.firewallSelectedInterval == 'last_day') {
            $scope.json_firewall.scaleX.values = timeLabels.filter((a,i) => i % 2 === 0);
        }
        else {
            if(timeLabels.length < PLOT_MAX_SAMPLES / 2)
                $scope.json_firewall.scaleX.values = timeLabels;
            else
                $scope.json_firewall.scaleX.values = timeLabels.slice(timeLabels.length - PLOT_MAX_SAMPLES / 2,
                                                                      timeLabels.length);
        }
    };

    $scope.reputationTimeLabelsUpdate = function() {
        if($scope.reputationSelectedInterval == 'last_day') {
            $scope.json_reputation.scaleX.values = $scope.reputationTimeLabels.filter((a,i) => i % 2 === 0);
        }
        else {
            if($scope.reputationTimeLabels.length < PLOT_MAX_SAMPLES / 2)
                $scope.json_reputation.scaleX.values = $scope.reputationTimeLabels;
            else
                $scope.json_reputation.scaleX.values = $scope.reputationTimeLabels.slice($scope.reputationTimeLabels.length - PLOT_MAX_SAMPLES / 2,
                                                                                		 $scope.reputationTimeLabels.length);
        }
    };

    $scope.firewallSelectedIntervalChanged = function() {
        if($scope.firewallSelectedDirection == 'egress') {
        	$scope.firewallEgressUpdatePlot();
        } else {
        	$scope.firewallIngressUpdatePlot();
        }
    };

    /* Callback function when selected interval for reputation graph is chanced */
    $scope.reputationSelectedIntervalChanged = function() {
        /* Update time labels for x-axis */
        $scope.reputationTimeLabelsUpdate();

        if($scope.reputationSelectedInterval == 'last_day') {
            /* Get PLOT_MAX_SAMPLES/2 samples from even positions */
            var filteredSystemReputationHistory = $scope.systemReputationHistory.filter((a,i) => i % 2 === 0);
            var filteredTemperatureReputationHistory = $scope.temperatureReputationHistory.filter((a,i) => i % 2 === 0);

            $scope.reputationValues[0] = filteredSystemReputationHistory;
            $scope.reputationValues[1] = filteredTemperatureReputationHistory;
        } else {
        	/* Last 12h (last PLOT_MAX_SAMPLES/2 values) */
            if($scope.systemReputationHistory.length < PLOT_MAX_SAMPLES / 2)
                $scope.reputationValues[0] = $scope.systemReputationHistory;
            else
            	$scope.reputationValues[0] = $scope.systemReputationHistory.slice($scope.systemReputationHistory.length - PLOT_MAX_SAMPLES / 2,
            																	  $scope.systemReputationHistory.length);
            
            if($scope.temperatureReputationHistory.length < PLOT_MAX_SAMPLES / 2)
                $scope.reputationValues[1] = $scope.temperatureReputationHistory;
            else
            	$scope.reputationValues[1] = $scope.temperatureReputationHistory.slice($scope.temperatureReputationHistory.length - PLOT_MAX_SAMPLES / 2,
            																		   $scope.temperatureReputationHistory.length);
        }
    };

   /* Configuration for firewall graph */
   $scope.json_firewall = {
      globals: {
          shadow: false,
          fontFamily: "Helvetica"
      },
      type: "line",
      legend: {
           layout: "x2",
           overflow: "page",
           alpha: 0.05,
           shadow: false,
           align: "right",
           marker: {
             type: "circle",
             borderColor: "none",
             size: "10px"
           },
           borderWidth: 0
      },
      plotarea:{
              margin: "dynamic",
      },
      scaleX: {
          zooming: true,
          label: {
              text: "Time"
          },
          guide: {
              visible: false
          },
          item:{
              fontAngle: -45,
              offsetX: "7px"
          }
      },
      scaleY: {
          label: {
              text: "Number of packets"
          }
      },
      tooltip: {
          visible: false
      },
      crosshairX: {
          scaleLabel: {
              backgroundColor: "#000000",
              fontColor: "white"
          },
          plotLabel: {
              backgroundColor: "black",
              fontColor: "#ffffff"
          }
      },
      plot: {
          aspect: "spline",
          marker: {
              visible: true
          }
      },
      series: [{
          text: "Passed Packets",
          lineColor: "#29A2CC"
      }, {
          text: "Dropped Packets",
          lineColor: "#D31E1E"
      }]
    };

   /* Configuration for reputation graph */
   $scope.json_reputation = {
      globals: {
          shadow: false,
          fontFamily: "Helvetica"
      },
      type: "line",
      legend: {
           layout: "x2",
           overflow: "page",
           alpha: 0.05,
           shadow: false,
           align: "right",
           marker: {
             type: "circle",
             borderColor: "none",
             size: "10px"
           },
           borderWidth: 0
      },
      plotarea:{
              margin: "dynamic",
      },
      scaleX: {
          zooming: true,
          label: {
              text: "Time"
          },
          guide: {
              visible: false
          },
          item:{
              fontAngle: -45,
              offsetX: "7px"
          }
      },
      scaleY: {
          label: {
              text: "Reputation value"
          }
      },
      tooltip: {
          visible: false
      },
      crosshairX: {
          scaleLabel: {
              backgroundColor: "#000000",
              fontColor: "white"
          },
          plotLabel: {
              backgroundColor: "black",
              fontColor: "#ffffff"
          }
      },
      plot: {
          aspect: "spline",
          marker: {
              visible: false
          }
      },
      series: [{
          text: "System reputation",
          lineColor: "#0B614B"
      }, {
          text: "Temperature reputation",
          lineColor: "#DF7401"
      }]
    };
}]);