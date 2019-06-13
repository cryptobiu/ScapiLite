package com.inellipse.biumatrix.service;

import com.inellipse.biumatrix.security.AuthenticatedUser;

public interface AuthService {
    AuthenticatedUser getAuthenticatedUser();

    String getAuthenticatedUserId();
}
