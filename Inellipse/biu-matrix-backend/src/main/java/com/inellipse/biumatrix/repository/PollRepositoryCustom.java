package com.inellipse.biumatrix.repository;

import com.inellipse.biumatrix.model.Poll;

import java.util.List;

public interface PollRepositoryCustom {

    List<Poll> getPollsWithClosedRegistration(long time);

}
