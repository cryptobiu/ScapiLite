package com.inellipse.biumatrix.exception;

public class ServiceException extends RuntimeException {

    private static final long serialVersionUID = 2931837790212992307L;

    private Integer code;
    private String message;

    public ServiceException() {
        super();
    }

    public ServiceException(Integer code, String message) {
        super(message);
        this.code = code;
        this.message = message;
    }

    public ServiceException(String message) {
        super(message);
        this.message = message;
    }

    public Integer getCode() {
        return code;
    }

    public String getMessage() {
        return message;
    }

}