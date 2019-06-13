package com.inellipse.biumatrix.exception;

public class AccessDeniedException extends RuntimeException {

    private static final long serialVersionUID = -4960089657720862471L;

    public AccessDeniedException() {
        super();
    }

    public AccessDeniedException(String message) {
        super(message);
    }

}