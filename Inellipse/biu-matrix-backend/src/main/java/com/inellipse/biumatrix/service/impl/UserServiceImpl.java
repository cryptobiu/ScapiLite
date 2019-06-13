package com.inellipse.biumatrix.service.impl;

import com.inellipse.biumatrix.dto.UserDTO;
import com.inellipse.biumatrix.model.User;
import com.inellipse.biumatrix.repository.UserRepository;
import com.inellipse.biumatrix.service.UserService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.stereotype.Service;

import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.stream.Collectors;

@Service
public class UserServiceImpl implements UserService {

    private final Logger logger = LoggerFactory.getLogger(getClass());

    private UserRepository userRepository;
    private PasswordEncoder passwordEncoder;

    @Autowired
    public UserServiceImpl(UserRepository userRepository, PasswordEncoder passwordEncoder) {
        this.userRepository = userRepository;
        this.passwordEncoder = passwordEncoder;
    }

    public UserDTO saveUser(UserDTO userDTO) {

        String newUsername = userDTO.getId() == null ? userDTO.getGoogleId() : userDTO.getId();

        Optional<User> optionalUser = userRepository.findByUsername(newUsername);
        User user;
        if (!optionalUser.isPresent()) {
            user = new User();
            user.setName(userDTO.getName());
            user.setEmail(userDTO.getEmail());
            user.setUsername(newUsername);
            user.setFacebookId(userDTO.getId());
            user.setRoles(Collections.singletonList("ROLE_USER"));
            user.setGoogleId(userDTO.getGoogleId());
            user.setGender(userDTO.getGender());
            user.setPassword(passwordEncoder.encode(userDTO.getPassword() == null ? "TEST123" : userDTO.getPassword()));
            user.setActive(true);
            user.setImage(userDTO.getImage());
            user = userRepository.save(user);
        } else {
            user = optionalUser.get();
        }
        return new UserDTO(user);
    }

    @Override
    public List<UserDTO> getAllUsers() {
        return userRepository.findAll().stream().map(UserDTO::new).collect(Collectors.toList());
    }
}
