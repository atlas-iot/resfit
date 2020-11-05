package ro.atlas.controller;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.LinkedList;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import ro.atlas.dto.AtlasClientCommandDto;
import ro.atlas.dto.AtlasOwnerCommandDto;
import ro.atlas.dto.AtlasOwnerFirebaseDto;
import ro.atlas.service.AtlasOwnerService;

@RestController
@RequestMapping("/atlas/owner")
public class AtlasOwnerController {
	private static final Logger LOG = LoggerFactory.getLogger(AtlasOwnerController.class);

	@Autowired
	private AtlasOwnerService ownerService;

	@GetMapping(path = "/probe")
	public String probeSaveFileController() {
		LOG.debug("Atlas owner controller is alive");
		DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");

		return dateFormat.format(new Date());
	}

	@GetMapping(path = "commands/{owner_identity}")
	public ResponseEntity<HashMap<String, LinkedList<AtlasClientCommandDto>>> getOwnerCommands(
			@PathVariable("owner_identity") String ownerIdentity) {
		LOG.info("Get commands for owner with identity {}", ownerIdentity);

		HashMap<String, LinkedList<AtlasClientCommandDto>> commands = ownerService.fetchOwnerCommands(ownerIdentity);

		return commands != null ? new ResponseEntity<>(commands, HttpStatus.OK)
				: new ResponseEntity<>(HttpStatus.NO_CONTENT);
	}

	@PostMapping(path = "commands/approve_status/{owner_identity}")
	public ResponseEntity<?> setOwnerCommandsStatus(@PathVariable("owner_identity") String ownerIdentity,
			@RequestBody AtlasOwnerCommandDto ownerCommand) {
		LOG.info("Set command status for owner with identity {}", ownerIdentity);

		boolean result = ownerService.setOwnerCommandStatus(ownerIdentity, ownerCommand);

		return result ? new ResponseEntity<>(HttpStatus.OK) : new ResponseEntity<>(HttpStatus.BAD_REQUEST);
	}
	
	@PostMapping(path = "firebase/{owner_identity}")
	public ResponseEntity<?> setOwnerFirebaseToken(@PathVariable("owner_identity") String ownerIdentity,
			@RequestBody AtlasOwnerFirebaseDto ownerFirebase) {
		LOG.info("Set firebase token for owner with identity {}", ownerIdentity);

		ownerService.updateFirebaseToken(ownerIdentity, ownerFirebase);

		return new ResponseEntity<>(HttpStatus.OK);
	}
}
