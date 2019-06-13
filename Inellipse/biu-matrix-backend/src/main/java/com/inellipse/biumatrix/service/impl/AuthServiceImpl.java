package com.inellipse.biumatrix.service.impl;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.inellipse.biumatrix.exception.UnauthorizedException;
import com.inellipse.biumatrix.security.AuthenticatedUser;
import com.inellipse.biumatrix.service.AuthService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.jwt.JwtHelper;
import org.springframework.security.oauth2.provider.OAuth2Authentication;
import org.springframework.security.oauth2.provider.authentication.OAuth2AuthenticationDetails;
import org.springframework.stereotype.Service;

@Service
public class AuthServiceImpl implements AuthService {

    @Autowired
    public AuthServiceImpl() {
    }

    @Override
    public AuthenticatedUser getAuthenticatedUser() {
        try {
            Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
            OAuth2Authentication oAuth2Authentication = ((OAuth2Authentication) authentication);
            String token = ((OAuth2AuthenticationDetails) oAuth2Authentication.getDetails()).getTokenValue();
            return new ObjectMapper().readValue(JwtHelper.decode(token).getClaims(), AuthenticatedUser.class);
        } catch (Exception e) {
            throw new UnauthorizedException("unable to get authenticated user");
        }
    }

    @Override
    public String getAuthenticatedUserId() {
        return getAuthenticatedUser().getUserId();
    }
}
