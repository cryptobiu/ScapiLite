package com.inellipse.biumatrix.exception;

public class UnauthorizedException extends RuntimeException {

    private static final long serialVersionUID = -7154914366832567862L;

    public UnauthorizedException() {
        super();
    }

    public UnauthorizedException(String message) {
        super(message);
    }

}
