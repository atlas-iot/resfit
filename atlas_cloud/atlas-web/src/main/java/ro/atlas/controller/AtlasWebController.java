package ro.atlas.controller;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import ro.atlas.dto.AtlasClientSummaryDto;
import ro.atlas.dto.AtlasGatewayAddDto;
import ro.atlas.entity.AtlasClient;
import ro.atlas.entity.AtlasGateway;
import ro.atlas.service.AtlasGatewayService;

@RestController
@RequestMapping("/atlas")
public class AtlasWebController {
    private static final Logger LOG = LoggerFactory.getLogger(AtlasWebController.class);
    private @Autowired
    AtlasGatewayService gatewayService;

    @GetMapping(path = "/probe")
    public String probeSaveFileController() {
        LOG.debug("Atlas controller is alive");
        DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");

        return dateFormat.format(new Date());
    }

    @GetMapping(path = "/gateways")
    public ResponseEntity<List<AtlasGateway>> listAllGateways() {
        LOG.debug("List all gateways GET request");

        List<AtlasGateway> gateways = gatewayService.getAllGateways();
        if (gateways.isEmpty()) {
            LOG.debug("There are no gateways");
            return new ResponseEntity<>(HttpStatus.NO_CONTENT);
        }

        return new ResponseEntity<>(gateways, HttpStatus.OK);
    }

    @PostMapping(path = "gateway/add")
    public ResponseEntity<?> addGateway(@RequestBody AtlasGatewayAddDto gatewayAddDto) {
        LOG.info("Add gateway with identity: " + gatewayAddDto.getIdentity() + " and psk: " + gatewayAddDto.getPsk());

        gatewayService.addGateway(gatewayAddDto);

        return new ResponseEntity<>(HttpStatus.CREATED);
    }

    @GetMapping(path = "gateway/clients/{gateway_identity}")
    public ResponseEntity<List<AtlasClientSummaryDto>> getGatewayClientsList(@PathVariable("gateway_identity") String gatewayIdentity) {
        LOG.debug("Fetching clients for gateway with identity: " + gatewayIdentity);

        List<AtlasClientSummaryDto> clients = gatewayService.getAllClientsSummary(gatewayIdentity);
        if (clients == null) {
            LOG.debug("There are no clients for gateway with identity " + gatewayIdentity);
            return new ResponseEntity<>(HttpStatus.NO_CONTENT);
        }

        return new ResponseEntity<>(clients, HttpStatus.OK);
    }

    @GetMapping(path = "gateway/client/{gateway_identity}/{client_identity}")
    public ResponseEntity<AtlasClient> getClientDetails(@PathVariable("gateway_identity") String gatewayIdentity, @PathVariable("client_identity") String clientIdentity) {
        LOG.debug("Fetching details for client with identity: " + clientIdentity);

        AtlasClient client = gatewayService.getClient(gatewayIdentity, clientIdentity);

        return new ResponseEntity<>(client, HttpStatus.OK);
    }

    @GetMapping(path = "gateway/force-sync/{gateway_identity}")
    public ResponseEntity<Void> forceSync(@PathVariable("gateway_identity") String gatewayIdentity) {
        LOG.info("Force sync for gateway with identity " + gatewayIdentity);

        AtlasGateway gateway = gatewayService.getGateway(gatewayIdentity);
        gatewayService.reqFullDeviceSync(gateway);

        return new ResponseEntity<Void>(HttpStatus.OK);
    }

    @DeleteMapping(path = "gateways/{gateway_identity}")
    public ResponseEntity<AtlasGateway> deleteGateway(@PathVariable("gateway_identity") String gatewayIdentity) {
        LOG.info("Deleting gateway with identity: " + gatewayIdentity + " from database!");

        AtlasGateway gateway = gatewayService.getGateway(gatewayIdentity);
        gatewayService.deleteGateway(gateway);

        return new ResponseEntity<>(HttpStatus.NO_CONTENT);
    }

    @DeleteMapping(path = "gateway/client/{gateway_identity}/{client_identity}")
    public ResponseEntity<AtlasClient> deleteGateway(@PathVariable("gateway_identity") String gatewayIdentity, @PathVariable("client_identity") String clientIdentity) {
        LOG.info("Deleting client with identity: " + clientIdentity + " from database!");

        AtlasGateway gateway = gatewayService.getGateway(gatewayIdentity);
        gatewayService.deleteClient(gateway, clientIdentity);

        return new ResponseEntity<>(HttpStatus.NO_CONTENT);
    }

    @PutMapping("gateway/client/{gateway_identity}/{client_identity}/{edited_alias}")
    public ResponseEntity<?> updateClientAlias(@PathVariable("gateway_identity") String gatewayIdentity, @PathVariable("client_identity") String clientIdentity, @PathVariable("edited_alias") String editedAlias) {
        LOG.info("Edit alias client with identity: " + clientIdentity + "with the new value " + editedAlias);
        gatewayService.updateClientAlias(gatewayIdentity, clientIdentity, editedAlias);

        return new ResponseEntity<>(HttpStatus.OK);
    }

    @PutMapping("gateway/client/command/{gateway_identity}/{client_identity}/{command}")
    public ResponseEntity<?> sendClientCommand(@PathVariable("gateway_identity") String gatewayIdentity, @PathVariable("client_identity") String clientIdentity, @PathVariable("command") String command) {
        LOG.info("Send command " + command + " to client with identity: " + clientIdentity + " (gateway identity: " + gatewayIdentity + ")");
        gatewayService.sendCommandToClient(gatewayIdentity, clientIdentity, command);

        return new ResponseEntity<>(HttpStatus.OK);
    }
}