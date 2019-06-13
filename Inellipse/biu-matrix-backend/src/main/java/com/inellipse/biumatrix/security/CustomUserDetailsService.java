package com.inellipse.biumatrix.security;

import com.inellipse.biumatrix.model.User;
import com.inellipse.biumatrix.repository.UserRepository;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.userdetails.UserDetails;
import org.springframework.security.core.userdetails.UserDetailsService;
import org.springframework.security.core.userdetails.UsernameNotFoundException;
import org.springframework.stereotype.Component;

import java.util.Optional;

@Component("userDetailsService")
public class CustomUserDetailsService implements UserDetailsService {

    private final Logger logger = LoggerFactory.getLogger(getClass());

    private final UserRepository userRepository;

    @Autowired
    public CustomUserDetailsService(UserRepository userRepository) {
        this.userRepository = userRepository;
    }

    @Override
    public UserDetails loadUserByUsername(final String username) {
        logger.debug("Authenticating {}", username);

        Optional<User> userOptional = userRepository.findByUsername(username);
        if (!userOptional.isPresent()) {
            logger.error("no user with username " + username + " was not found in the database!");
            throw new UsernameNotFoundException("User " + username + " was not found in the database!");
        }

        User user = userOptional.get();
        if (!user.isActive()) {
            logger.error("User " + username + " is not active!");
            throw new UsernameNotFoundException("User " + username + " is not active!");
        }

        return new CustomUserDetails(user.getUsername(), user.getPassword(), user.isActive(), user.getRoles());
    }
}

