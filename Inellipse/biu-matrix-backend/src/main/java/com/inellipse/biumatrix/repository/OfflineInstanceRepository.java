package com.inellipse.biumatrix.repository;

import com.inellipse.biumatrix.model.OfflineInstance;
import org.springframework.data.mongodb.repository.MongoRepository;

import java.util.List;

public interface OfflineInstanceRepository extends MongoRepository<OfflineInstance, String> {

    OfflineInstance findByCid(String cid);
    List<OfflineInstance> findByPollIdInAndAnswer(List<String> pollIds, Boolean answer);

}
