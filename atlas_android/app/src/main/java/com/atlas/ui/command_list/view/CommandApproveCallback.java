package com.atlas.ui.command_list.view;

import com.atlas.model.database.AtlasCommand;

public interface CommandApproveCallback {
    void onApproveButtonClick(AtlasCommand command);

    void onRejectButtonClick(AtlasCommand command);
}
