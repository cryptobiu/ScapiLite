package com.inellipse.biumatrix.repository;

import com.inellipse.biumatrix.model.User;
import org.springframework.data.mongodb.repository.MongoRepository;

import java.util.List;
import java.util.Optional;

public interface UserRepository extends MongoRepository<User, String> {
    Optional<User> findByUsername(String username);
    Optional<User> findByFacebookId(String facebookId);
    List<User> findByIdIn(List<String> ids);
}
