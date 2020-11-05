package com.atlas.ui.gateway_list.view;

import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;

public class BackStackFragment extends Fragment {

    public static boolean handleBackPressed(FragmentManager fm) {
        fm.getFragments();
        for (Fragment frag : fm.getFragments()) {
            if (frag != null && frag.isVisible() && frag instanceof BackStackFragment) {
                if (((BackStackFragment) frag).onBackPressed()) {
                    return true;
                }
            }
        }
        return false;
    }

    protected boolean onBackPressed() {
        FragmentManager fm = getChildFragmentManager();
        if (handleBackPressed(fm)) {
            return true;
        } else if (fm.getBackStackEntryCount() > 0) {
            fm.popBackStack();
            return true;
        }
        return false;
    }
}
