package tqsgroup.chuchu.data.entity;

import jakarta.persistence.*;
import jakarta.validation.constraints.NotNull;
import jakarta.validation.constraints.Size;
import jakarta.validation.constraints.Min;
import jakarta.validation.constraints.Max;

@Entity
@Table(name = "STATIONS")
public class Station {

    private static final int MIN_STATION_NAME_LENGTH = 3;
    private static final int MAX_STATION_NAME_LENGTH = 255;
    private static final int MIN_NUMBER_OF_LINES = 1;
    private static final int MAX_NUMBER_OF_LINES = 30;

    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Column(name = "name", unique = true)
    @Size(min = MIN_STATION_NAME_LENGTH, max = MAX_STATION_NAME_LENGTH)
    @NotNull
    private String name;

    @Column(name = "lines", columnDefinition = "INTEGER")
    @Min(MIN_NUMBER_OF_LINES)
    @Max(MAX_NUMBER_OF_LINES)
    @NotNull
    private int numberOfLines; //Should be equivalent to the number of screens

    public Station() {
    }

    public Station(String name, int numberOfLines) {
        this.name = name;
        this.numberOfLines = numberOfLines;
    }

    public Long getId() {
        return id;
    }
    
    public void setId(Long id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getNumberOfLines() {
        return numberOfLines;
    }

    public void setNumberOfLines(int numberOfLines) {
        this.numberOfLines = numberOfLines;
    }
}
