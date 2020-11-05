package ro.atlas.service.impl;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import ro.atlas.dto.AtlasClientCommandDto;
import ro.atlas.dto.AtlasOwnerCommandDto;
import ro.atlas.dto.AtlasOwnerFirebaseDto;
import ro.atlas.entity.AtlasOwner;
import ro.atlas.repository.AtlasOwnerRepository;
import ro.atlas.service.AtlasGatewayService;
import ro.atlas.service.AtlasOwnerService;

@Component
public class AtlasOwnerServiceImpl implements AtlasOwnerService {

	private static final Logger LOG = LoggerFactory.getLogger(AtlasOwnerServiceImpl.class);

	private @Autowired AtlasOwnerRepository ownerRepository;
	private @Autowired AtlasGatewayService gatewayService;
	private @Autowired FirebaseServiceImpl firebaseService;

	@Override
	public void initOwners() {
		LOG.debug("Init owners service");

		/* Send a notification to each owner which has at least one pending command */
		List<AtlasOwner> owners = ownerRepository.findAll();
		owners.forEach((owner) -> {
			if (owner.getOwnerCommands() != null && !owner.getOwnerCommands().isEmpty()) {
				sendFirebaseNotification(owner);
			}
		});
	}
	
	@Override
	public synchronized void enqueueOwnerCommand(String gatewayIdentity, String ownerIdentity,
			AtlasClientCommandDto clientCommand) {
		/* Sanity check */
		if (gatewayIdentity == null || gatewayIdentity.isEmpty()) {
			LOG.error("Empty gateway identity!");
			return;
		}
		if (ownerIdentity == null || ownerIdentity.isEmpty()) {
			LOG.error("Empty owner identity!");
			return;
		}
		if (clientCommand == null) {
			LOG.error("Empty client command!");
			return;
		}

		/* Save owner command to database */
		AtlasOwner owner = ownerRepository.findByOwnerIdentity(ownerIdentity);
		if (owner == null) {
			owner = new AtlasOwner();
			owner.setOwnerIdentity(ownerIdentity);
			owner.setOwnerCommands(new HashMap<String, LinkedList<AtlasClientCommandDto>>());
			owner = ownerRepository.insert(owner);
		}

		/* Add client command */
		if (owner.getOwnerCommands().get(gatewayIdentity) == null) {
			owner.getOwnerCommands().put(gatewayIdentity, new LinkedList<AtlasClientCommandDto>());
		}
		owner.getOwnerCommands().get(gatewayIdentity).add(clientCommand);
		owner.setNotifyUser(true);

		LOG.info("Save command for owner with identity {} and gateway with identity {}", ownerIdentity,
				gatewayIdentity);

		/* Save owner information */
		owner = ownerRepository.save(owner);
		
		/* Send firebase notification to owner */
		sendFirebaseNotification(owner);
	}

	@Override
	public synchronized HashMap<String, LinkedList<AtlasClientCommandDto>> fetchOwnerCommands(String ownerIdentity) {
		if (ownerIdentity == null || ownerIdentity.isEmpty()) {
			return null;
		}

		LOG.info("Fetch owner commands for owner with identity {}", ownerIdentity);

		AtlasOwner owner = ownerRepository.findByOwnerIdentity(ownerIdentity);
		if (owner == null) {
			LOG.error("Cannot find owner with identity {}", ownerIdentity);
			return null;
		}

		return owner.getOwnerCommands();
	}

	@Override
	public synchronized boolean setOwnerCommandStatus(String ownerIdentity, AtlasOwnerCommandDto ownerCommand) {
		if (ownerIdentity == null || ownerIdentity.isEmpty()) {
			LOG.error("Cannot process owner status command with empty owner identity!");
			return false;
		}

		if (ownerCommand == null) {
			LOG.error("Cannot process empty owner status command!");
			return false;
		}

		LOG.info("Set owner status command with sequence number {} for owner with identity {}", ownerCommand.getSeqNo(),
				ownerIdentity);

		AtlasOwner owner = ownerRepository.findByOwnerIdentity(ownerIdentity);
		if (owner == null) {
			LOG.error("Cannot find owner with identity {}", ownerIdentity);
			return false;
		}

		LinkedList<AtlasClientCommandDto> commands = owner.getOwnerCommands().get(ownerCommand.getGatewayIdentity());
		if (commands == null || commands.isEmpty()) {
			LOG.info("Cannot find command list for gateway with identity {}. Seems like an already processed command. Send success to owner...", ownerCommand.getGatewayIdentity());
			return true;
		}

		/*
		 * Get first client command from the gateway command list (owner MUST
		 * approve/reject the first command)
		 */
		AtlasClientCommandDto clientCommand = commands.get(0);
		/*
		 * If the owner sends an old command which already was approved, then
		 * acknowledge this as success
		 */
		if (ownerCommand.getSeqNo() < clientCommand.getSeqNo()) {
			LOG.info(
					"Command with sequence number {} for owner with identity {} and gateway with identity {} seems like an already processed command. Send success to owner...",
					ownerCommand.getSeqNo(), ownerIdentity, ownerCommand.getGatewayIdentity());
			return true;
		}

		if (clientCommand.getSeqNo() != ownerCommand.getSeqNo()) {
			LOG.error(
					"Status command for owner with identity {} and gateway with identity {} is be rejected: sequence number mismatch",
					ownerIdentity, ownerCommand.getGatewayIdentity());
			return false;
		}

		if (!clientCommand.getClientIdentity().equalsIgnoreCase(ownerCommand.getClientIdentity())) {
			LOG.error(
					"Status command for owner with identity {} and gateway with identity {} is be rejected: client identity mismatch",
					ownerIdentity, ownerCommand.getGatewayIdentity());
			return false;
		}
		
		/*
		 * If command is approved, then send it on the gateway side which in turn will
		 * send it to the client
		 */
		if (ownerCommand.isApproved()) {
			LOG.info(
					"Sending approved command with sequence number {} to gateway for owner with identity {} and gateway with identity {}",
					ownerCommand.getSeqNo(), ownerIdentity, ownerCommand.getGatewayIdentity());
			
			if (ownerCommand.getSignature() == null || ownerCommand.getSignature().isEmpty()) {
				LOG.info(
						"Approved command with sequence number {} to gateway for owner with identity {} and gateway with identity {} is rejected: empty signature",
						ownerCommand.getSeqNo(), ownerIdentity, ownerCommand.getGatewayIdentity());

				return false;
			}
			
			/* Remove command from owner list and move it to gateway list */
			clientCommand.setSignature(ownerCommand.getSignature());
			if (!gatewayService.sendApprovedCommandToClient(ownerCommand.getGatewayIdentity(), clientCommand)) {
				LOG.error(
						"Cannot insert approved command in the database for owner with identity {} and gateway with identity {}",
						ownerIdentity, ownerCommand.getGatewayIdentity());
				return false;
			}
		} else {
			LOG.info(
					"Deleting rejected command with sequence number {} for owner with identity {} and gateway with identity {}",
					ownerCommand.getSeqNo(), ownerIdentity, ownerCommand.getGatewayIdentity());
			
			if (!gatewayService.markCommandAsRejected(ownerCommand.getGatewayIdentity(), clientCommand)) {
				LOG.error(
						"Cannot mark command as rejected for owner with identity {} and gateway with identity {}",
						ownerIdentity, ownerCommand.getGatewayIdentity());
				return false;
			}
		}

		/* Remove command from the owner list */
		commands.remove(0);
		if (commands.isEmpty()) {
			owner.getOwnerCommands().remove(ownerCommand.getGatewayIdentity());
		}
		
		ownerRepository.save(owner);

		return true;
	}

	@Override
	public synchronized void updateFirebaseToken(String ownerIdentity, AtlasOwnerFirebaseDto ownerFirebase) {
		/* Sanity check */
		if (ownerIdentity == null || ownerIdentity.isEmpty()) {
			LOG.error("Empty owner identity!");
			return;
		}
		if (ownerFirebase == null || ownerFirebase.getFirebaseToken() == null || ownerFirebase.getFirebaseToken().isEmpty()) {
			LOG.error("Empty firebase token!");
			return;
		}

		/* Save owner command to database */
		AtlasOwner owner = ownerRepository.findByOwnerIdentity(ownerIdentity);
		if (owner == null) {
			owner = new AtlasOwner();
			owner.setOwnerIdentity(ownerIdentity);
			owner.setOwnerCommands(new HashMap<String, LinkedList<AtlasClientCommandDto>>());
			owner = ownerRepository.insert(owner);
		}
		
		owner.setFirebaseToken(ownerFirebase.getFirebaseToken());
		ownerRepository.save(owner);
	}
	
	private void sendFirebaseNotification(AtlasOwner owner) {
		LOG.info("Sending firebase notification to owner with identity {}", owner.getOwnerIdentity());
		
		if (owner.getFirebaseToken() == null || owner.getFirebaseToken().isEmpty()) {
			LOG.error("Cannot send firebase notification to empty token");
			return;
		}
		
		firebaseService.sendPushNotification(owner.getFirebaseToken());
	}
}
