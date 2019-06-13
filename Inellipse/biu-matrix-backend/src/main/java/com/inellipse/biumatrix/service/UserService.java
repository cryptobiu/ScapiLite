package com.inellipse.biumatrix.service;

import com.inellipse.biumatrix.dto.UserDTO;

import java.util.List;

public interface UserService {
    UserDTO saveUser(UserDTO user);

    List<UserDTO> getAllUsers();
}
