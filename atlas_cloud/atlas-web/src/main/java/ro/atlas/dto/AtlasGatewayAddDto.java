package ro.atlas.dto;

public class AtlasGatewayAddDto {
    /* Gateway identity */
    private String identity;

    /* Gateway PSK */
    private String psk;

    /* Gateway alias */
    private String alias;

    public AtlasGatewayAddDto() {
        this.identity = "";
        this.psk = "";
        this.alias = "";
    }

    public AtlasGatewayAddDto(String identity, String psk, String alias) {
        this.identity = identity;
        this.psk = psk;
        this.alias = alias;
    }

    public String getIdentity() {
        return identity;
    }

    public void setIdentity(String identity) {
        this.identity = identity;
    }

    public String getPsk() {
        return psk;
    }

    public void setPsk(String psk) {
        this.psk = psk;
    }

    public String getAlias() {
        return alias;
    }

    public void setAlias(String alias) {
        this.alias = alias;
    }
}
