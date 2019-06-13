package com.inellipse.biumatrix.service;

import com.inellipse.biumatrix.dto.PollDTO;

public interface PollDetailService {
    PollDTO savePollDetails(String pollId, String userId, String status);
}
