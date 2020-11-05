package com.atlas.networking;

import com.atlas.model.dto.AtlasClientCommandsResp;
import com.atlas.model.dto.AtlasGatewayClaimReq;
import com.atlas.model.dto.AtlasOwnerFirebase;

import java.util.List;
import java.util.Map;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.Path;

public interface AtlasFirebaseAPI {
    @POST("atlas/owner/firebase/{owner_identity}")
    Call<Void> updateFirebaseToken(@Path("owner_identity") String ownerIdentity, @Body AtlasOwnerFirebase ownerFirebase);
}
