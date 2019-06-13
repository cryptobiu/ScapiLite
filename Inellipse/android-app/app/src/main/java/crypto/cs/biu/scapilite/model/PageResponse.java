package crypto.cs.biu.scapilite.model;

import java.util.List;

/**
 * Created by Blagojco on 03/05/2018- 16:42
 */

public class PageResponse {

    private List<Poll> content;

    public List<Poll> getContent() {
        return content;
    }

    public void setContent(List<Poll> content) {
        this.content = content;
    }
}
