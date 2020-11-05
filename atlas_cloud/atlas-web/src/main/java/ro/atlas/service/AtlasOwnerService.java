package ro.atlas.service;

import java.util.HashMap;
import java.util.LinkedList;

import org.springframework.stereotype.Service;

import ro.atlas.dto.AtlasClientCommandDto;
import ro.atlas.dto.AtlasOwnerCommandDto;
import ro.atlas.dto.AtlasOwnerFirebaseDto;

@Service
public interface AtlasOwnerService {

	/**
	 * Init owners service
	 */
	public void initOwners();
	
	/**
	 * Enqueue owner command. This method saves the owner command in a list and
	 * transmits the command to the owner via push notifications.
	 * @param gatewayIdentity Gateway identity 
	 * @param ownerIdentity Owner identifier (used for push notifications)
	 * @param clientCommand Client command
	 */
	public void enqueueOwnerCommand(String gatewayIdentity, String ownerIdentity, AtlasClientCommandDto clientCommand);
	
	/**
	 * Fetch all owner pending commands
	 * @param ownerIdentity Owner identity
	 * @return Owner commands
	 */
	public HashMap<String, LinkedList<AtlasClientCommandDto>> fetchOwnerCommands(String ownerIdentity);
	
	/**
	 * Set owner command approved status (if approved the command will be sent to the gateway)
	 * @param ownerCommand Owner command approved status
	 */
	public boolean setOwnerCommandStatus(String ownerIdentity, AtlasOwnerCommandDto ownerCommand);
	
	/**
	 * Update firebase token for an owner
	 * @param ownerIdentity Owner identity
	 * @param ownerFirebase Firebase token
	 */
	public void updateFirebaseToken(String ownerIdentity, AtlasOwnerFirebaseDto ownerFirebase);
}
