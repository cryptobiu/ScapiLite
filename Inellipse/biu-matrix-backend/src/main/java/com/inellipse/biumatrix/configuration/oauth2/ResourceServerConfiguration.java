package com.inellipse.biumatrix.configuration.oauth2;

import com.inellipse.biumatrix.security.CustomAuthenticationEntryPoint;
import com.inellipse.biumatrix.security.CustomLogoutSuccessHandler;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.Primary;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.http.SessionCreationPolicy;
import org.springframework.security.oauth2.config.annotation.web.configuration.EnableResourceServer;
import org.springframework.security.oauth2.config.annotation.web.configuration.ResourceServerConfigurerAdapter;
import org.springframework.security.oauth2.config.annotation.web.configurers.ResourceServerSecurityConfigurer;
import org.springframework.security.oauth2.provider.token.DefaultTokenServices;
import org.springframework.security.oauth2.provider.token.TokenStore;
import org.springframework.security.oauth2.provider.token.store.JwtAccessTokenConverter;
import org.springframework.security.oauth2.provider.token.store.JwtTokenStore;
import org.springframework.security.web.util.matcher.AntPathRequestMatcher;

@Configuration
@EnableResourceServer
public class ResourceServerConfiguration extends ResourceServerConfigurerAdapter {

    private final CustomAuthenticationEntryPoint authEntryPoint;
    private final CustomLogoutSuccessHandler logoutSuccessHandler;

    @Value("${oauth.jwt.secret}")
    private String jwtSecret;

    @Autowired
    public ResourceServerConfiguration(CustomAuthenticationEntryPoint authEntryPoint, CustomLogoutSuccessHandler logoutSuccessHandler) {
        this.authEntryPoint = authEntryPoint;
        this.logoutSuccessHandler = logoutSuccessHandler;
    }

    @Override
    public void configure(ResourceServerSecurityConfigurer config) {
        config.tokenServices(tokenServices());
    }

    @Bean
    public TokenStore tokenStore() {
        return new JwtTokenStore(accessTokenConverter());
    }

    @Bean
    public JwtAccessTokenConverter accessTokenConverter() {
        JwtAccessTokenConverter converter = new JwtAccessTokenConverter();
        converter.setSigningKey(jwtSecret);
        return converter;
    }

    @Bean
    @Primary
    public DefaultTokenServices tokenServices() {
        DefaultTokenServices defaultTokenServices = new DefaultTokenServices();
        defaultTokenServices.setTokenStore(tokenStore());
        return defaultTokenServices;
    }

    @Override
    public void configure(HttpSecurity http) throws Exception {
        http.exceptionHandling()
                .authenticationEntryPoint(authEntryPoint)
                .and()
                .logout().logoutUrl("/oauth/logout").logoutSuccessHandler(logoutSuccessHandler)
                .and()
                .csrf().requireCsrfProtectionMatcher(new AntPathRequestMatcher("/oauth/authorize")).disable()
                .headers().frameOptions().disable()
                .and()
                .sessionManagement()
                .sessionCreationPolicy(SessionCreationPolicy.STATELESS)
                .and()
                .authorizeRequests()
                // swagger
                .antMatchers("/v2/api-docs/**",
                        "/configuration/ui/**",
                        "/swagger-resources/**",
                        "/configuration/security/**",
                        "/swagger-ui.html/**",
                        "/webjars/**").permitAll()
                // api
                .antMatchers("/external/**").permitAll()
                .antMatchers("/cs/**").permitAll()
                .antMatchers("/**").hasAnyAuthority("ROLE_USER");
    }
}
