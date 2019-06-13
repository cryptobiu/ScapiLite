package com.inellipse.biumatrix.configuration.build;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import springfox.documentation.builders.PathSelectors;
import springfox.documentation.builders.RequestHandlerSelectors;
import springfox.documentation.service.ApiInfo;
import springfox.documentation.service.Contact;
import springfox.documentation.service.VendorExtension;
import springfox.documentation.spi.DocumentationType;
import springfox.documentation.spring.web.plugins.Docket;
import springfox.documentation.swagger2.annotations.EnableSwagger2;

import java.util.Collections;
import java.util.List;

@Configuration
@EnableSwagger2
public class SwaggerConfiguration {

    private static final String BASE_PACKAGE = "com.inellipse.biumatrix.controller";
    private static final String TITLE = "BIUMatrix API";
    private static final String DESCRIPTION = "REST API for BIUMatrix";
    private static final String VERSION = "1.0";
    private static final String TERMS_OF_SERVICE_URL = "/";
    private static final Contact CONTACT = ApiInfo.DEFAULT_CONTACT;
    private static final String LICENCE = "";
    private static final String LICENCE_URL = "";
    private static final List<VendorExtension> VENDOR_EXTENSIONS = Collections.emptyList();

    @Bean
    public Docket apiDocket() {
        return new Docket(DocumentationType.SWAGGER_2)
                .select()
                .apis(RequestHandlerSelectors.basePackage(BASE_PACKAGE))
                .paths(PathSelectors.any())
                .build()
                .apiInfo(metaData());
    }

    private ApiInfo metaData() {
        return new ApiInfo(TITLE, DESCRIPTION, VERSION,
                TERMS_OF_SERVICE_URL, CONTACT, LICENCE,
                LICENCE_URL, VENDOR_EXTENSIONS);
    }
}