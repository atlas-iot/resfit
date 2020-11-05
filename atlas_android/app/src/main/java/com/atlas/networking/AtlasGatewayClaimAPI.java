package com.atlas.networking;

import com.atlas.model.dto.AtlasGatewayClaimReq;
import com.atlas.model.dto.AtlasGatewayClaimResp;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.POST;

public interface AtlasGatewayClaimAPI {
    @POST("gateway/claim")
    Call<AtlasGatewayClaimResp> claimGateway(@Body AtlasGatewayClaimReq claimReq);
}
