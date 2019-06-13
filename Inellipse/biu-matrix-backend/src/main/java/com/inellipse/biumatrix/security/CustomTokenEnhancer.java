package com.inellipse.biumatrix.security;

import com.inellipse.biumatrix.model.User;
import com.inellipse.biumatrix.repository.UserRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.oauth2.common.DefaultOAuth2AccessToken;
import org.springframework.security.oauth2.common.OAuth2AccessToken;
import org.springframework.security.oauth2.provider.OAuth2Authentication;
import org.springframework.security.oauth2.provider.token.TokenEnhancer;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

public class CustomTokenEnhancer implements TokenEnhancer {

    private final UserRepository userRepository;

    @Autowired
    public CustomTokenEnhancer(UserRepository userRepository) {
        this.userRepository = userRepository;
    }

    @Override
    public OAuth2AccessToken enhance(OAuth2AccessToken accessToken, OAuth2Authentication authentication) {
        Optional<User> userOptional = userRepository.findByUsername(authentication.getName());
        if (!userOptional.isPresent()) {
            // should not happen
            return null;
        }

        Map<String, Object> additionalInfo = new HashMap<>();

        User user = userOptional.get();
        additionalInfo.put("user_id", user.getId());
        additionalInfo.put("username", user.getName());

        DefaultOAuth2AccessToken customAccessToken = new DefaultOAuth2AccessToken(accessToken);
        customAccessToken.setAdditionalInformation(additionalInfo);
        return customAccessToken;
    }
}
