package com.atlas.networking;

import com.atlas.model.dto.AtlasClientCommandsResp;
import com.atlas.model.dto.AtlasOwnerCommandReq;

import java.util.List;
import java.util.Map;

import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.POST;
import retrofit2.http.Path;

public interface AtlasClientCommandAPI {
    @GET("atlas/owner/commands/{owner_identity}")
    Call<Map<String, List<AtlasClientCommandsResp>>> getClientCommands(@Path("owner_identity") String ownerIdentity);

    @POST("atlas/owner/commands/approve_status/{owner_identity}")
    Call<ResponseBody> sendCommandStatus(@Path("owner_identity") String ownerIdentity, @Body AtlasOwnerCommandReq ownerCommand);
}
