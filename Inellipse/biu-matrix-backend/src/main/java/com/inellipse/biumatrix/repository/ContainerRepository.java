package com.inellipse.biumatrix.repository;

import com.inellipse.biumatrix.model.Container;
import org.springframework.data.mongodb.repository.MongoRepository;

public interface ContainerRepository extends MongoRepository<Container, String> {
    Container findByExternalId(String externalId);
}
