package com.atlas.ui.gateway_list.view;


import com.atlas.model.database.AtlasGateway;

public interface  GatewayClickCallback {
    void onClick(AtlasGateway gateway);
    boolean onLongClick(AtlasGateway gateway);
}
