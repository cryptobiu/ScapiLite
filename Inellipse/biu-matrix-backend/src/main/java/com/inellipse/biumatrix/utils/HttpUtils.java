package com.inellipse.biumatrix.utils;

import javax.servlet.http.HttpServletRequest;

public class HttpUtils {

    private static final String X_FORWARDER_FOR = "X-FORWARDED-FOR";

    public static String getIpAddress(HttpServletRequest request) {
        if (request.getHeader(X_FORWARDER_FOR) != null) {
            return request.getHeader(X_FORWARDER_FOR);
        }
        return request.getRemoteAddr();
    }
}
