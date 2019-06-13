package com.inellipse.biumatrix.service;

import com.inellipse.biumatrix.model.OfflineInstance;
import com.inellipse.biumatrix.model.Poll;

public interface MatrixService {

    void openForRegistration(String pollName);
    void registerToPoll(Poll poll, OfflineInstance offlineInstance);
    void closePollForRegistration();

}
