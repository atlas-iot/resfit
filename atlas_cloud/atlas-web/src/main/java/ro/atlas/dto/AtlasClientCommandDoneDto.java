package ro.atlas.dto;

public class AtlasClientCommandDoneDto {
	/* Command sequence number */
	private int seqNo;
	
	/* Client identity */
	private String clientIdentity;

	public int getSeqNo() {
		return seqNo;
	}

	public void setSeqNo(int seqNo) {
		this.seqNo = seqNo;
	}

	public String getClientIdentity() {
		return clientIdentity;
	}

	public void setClientIdentity(String clientIdentity) {
		this.clientIdentity = clientIdentity;
	}
}
