package com.inellipse.biumatrix.controller;

import com.inellipse.biumatrix.dto.UserDTO;
import com.inellipse.biumatrix.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class UserController {

    private UserService userService;

    @Autowired
    public UserController(UserService userService) {
        this.userService = userService;
    }

    @PostMapping(value = "/external/users")
    public UserDTO saveUser(@RequestBody UserDTO user) {
        return userService.saveUser(user);
    }

    @GetMapping(value = "/users")
    public List<UserDTO> getAllUsers() {
        return userService.getAllUsers();
    }
}
