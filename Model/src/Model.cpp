#include "Model.h"

Model::Model(QObject *parent)
    : QObject(parent), currentUnitIndex(-1), currentProblemIndex(-1)
{
    initializeSampleData();
}

void Model::addUnit(const Unit& unit)
{
    units.append(unit);
    emit unitsChanged();
}

void Model::removeUnit(int index)
{
    if (index >= 0 && index < units.size()) {
        units.removeAt(index);
        emit unitsChanged();
    }
}

const QVector<Unit>& Model::getUnits() const
{
    return units;
}

Unit* Model::getUnit(int index)
{
    if (index >= 0 && index < units.size()) {
        return &units[index];
    }
    return nullptr;
}

int Model::getUnitCount() const
{
    return units.size();
}

void Model::addProblemToUnit(int unitIndex, const Problem& problem)
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        units[unitIndex].addProblem(problem);
        emit unitProblemsChanged(unitIndex);
    }
}

void Model::removeProblemFromUnit(int unitIndex, int problemIndex)
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        if (problemIndex >= 0 && problemIndex < units[unitIndex].problems.size()) {
            units[unitIndex].problems.removeAt(problemIndex);
            emit unitProblemsChanged(unitIndex);
        }
    }
}

QStringList Model::getProblemsForUnit(int unitIndex) const
{
    QStringList problemNames;
    if (unitIndex >= 0 && unitIndex < units.size()) {
        for (const Problem& problem : units[unitIndex].problems) {
            problemNames.append(problem.name);
        }
    }
    return problemNames;
}

void Model::initializeSampleData()
{
    // Unit 1: Basic Mathematics
    Unit mathUnit("Basic Mathematics", "Fundamental mathematical concepts and operations");
    
    Problem addSub("Addition & Subtraction", "Basic arithmetic operations", "Easy");
    addSub.problemStatement = "Calculate: 15 + 8 - 3 = ?";
    addSub.theoryContent = "Addition and Subtraction Theory\n\nAddition combines numbers to find their total sum.\nSubtraction finds the difference between numbers.\n\nOrder of operations: Work from left to right for addition and subtraction of the same level.\n\nExample: 15 + 8 - 3\nStep 1: 15 + 8 = 23\nStep 2: 23 - 3 = 20";
    addSub.choices = {
        MultipleChoiceOption("A) 18", false),
        MultipleChoiceOption("B) 20", true),
        MultipleChoiceOption("C) 22", false),
        MultipleChoiceOption("D) 26", false)
    };
    
    Problem multDiv("Multiplication & Division", "Basic multiplication and division", "Easy");
    multDiv.problemStatement = "Calculate: 24 ÷ 4 × 3 = ?";
    multDiv.theoryContent = "Multiplication and Division Theory\n\nMultiplication is repeated addition.\nDivision splits a number into equal parts.\n\nOrder of operations: Multiplication and division have equal priority, work left to right.\n\nExample: 24 ÷ 4 × 3\nStep 1: 24 ÷ 4 = 6\nStep 2: 6 × 3 = 18";
    multDiv.choices = {
        MultipleChoiceOption("A) 2", false),
        MultipleChoiceOption("B) 18", true),
        MultipleChoiceOption("C) 72", false),
        MultipleChoiceOption("D) 288", false)
    };
    
    Problem fractions("Fractions", "Working with fractions and decimals", "Medium");
    fractions.problemStatement = "Calculate: 1/2 + 1/4 = ?";
    fractions.theoryContent = "Fractions Theory\n\nTo add fractions, find a common denominator.\nThe least common multiple of 2 and 4 is 4.\n\n1/2 = 2/4\n1/4 = 1/4\n\n2/4 + 1/4 = 3/4";
    fractions.choices = {
        MultipleChoiceOption("A) 1/6", false),
        MultipleChoiceOption("B) 2/6", false),
        MultipleChoiceOption("C) 3/4", true),
        MultipleChoiceOption("D) 1/8", false)
    };
    
    mathUnit.addProblem(addSub);
    mathUnit.addProblem(multDiv);
    mathUnit.addProblem(fractions);
    mathUnit.addProblem(Problem("Percentages", "Calculating percentages and ratios", "Medium"));
    
    // Unit 2: Algebra
    Unit algebraUnit("Algebra", "Introduction to algebraic concepts");
    
    Problem linearEq("Linear Equations", "Solving linear equations", "Medium");
    linearEq.problemStatement = "Solve for x: 2x + 5 = 13";
    linearEq.theoryContent = "Linear Equations Theory\n\nA linear equation has the form ax + b = c\n\nSteps to solve:\n1. Isolate the variable term\n2. Divide by the coefficient\n\nExample: 2x + 5 = 13\nStep 1: 2x = 13 - 5 = 8\nStep 2: x = 8 ÷ 2 = 4";
    linearEq.choices = {
        MultipleChoiceOption("A) x = 4", true),
        MultipleChoiceOption("B) x = 6", false),
        MultipleChoiceOption("C) x = 8", false),
        MultipleChoiceOption("D) x = 9", false)
    };
    
    Problem quadEq("Quadratic Equations", "Solving quadratic equations", "Hard");
    quadEq.problemStatement = "Solve: x² - 5x + 6 = 0";
    quadEq.theoryContent = "Quadratic Equations Theory\n\nQuadratic equations have the form ax² + bx + c = 0\n\nMethods to solve:\n1. Factoring\n2. Quadratic formula\n\nFor x² - 5x + 6 = 0:\nFactor: (x - 2)(x - 3) = 0\nSolutions: x = 2 or x = 3";
    quadEq.choices = {
        MultipleChoiceOption("A) x = 1, 6", false),
        MultipleChoiceOption("B) x = 2, 3", true),
        MultipleChoiceOption("C) x = -2, -3", false),
        MultipleChoiceOption("D) x = 0, 5", false)
    };
    
    algebraUnit.addProblem(linearEq);
    algebraUnit.addProblem(quadEq);
    algebraUnit.addProblem(Problem("Systems of Equations", "Solving systems of linear equations", "Hard"));
    algebraUnit.addProblem(Problem("Polynomials", "Working with polynomial expressions", "Medium"));
    
    // Unit 3: Geometry
    Unit geometryUnit("Geometry", "Basic geometric shapes and calculations");
    
    Problem areaPerim("Area & Perimeter", "Calculating area and perimeter", "Easy");
    areaPerim.problemStatement = "Find the area of a rectangle with length 8 cm and width 5 cm.";
    areaPerim.theoryContent = "Area and Perimeter Theory\n\nRectangle formulas:\nArea = length × width\nPerimeter = 2(length + width)\n\nFor length = 8 cm, width = 5 cm:\nArea = 8 × 5 = 40 cm²";
    areaPerim.choices = {
        MultipleChoiceOption("A) 26 cm²", false),
        MultipleChoiceOption("B) 40 cm²", true),
        MultipleChoiceOption("C) 13 cm²", false),
        MultipleChoiceOption("D) 80 cm²", false)
    };
    
    geometryUnit.addProblem(areaPerim);
    geometryUnit.addProblem(Problem("Triangles", "Properties and calculations of triangles", "Medium"));
    geometryUnit.addProblem(Problem("Circles", "Circle properties and calculations", "Medium"));
    geometryUnit.addProblem(Problem("3D Shapes", "Volume and surface area of 3D shapes", "Hard"));
    
    // Unit 4: Statistics
    Unit statsUnit("Statistics", "Basic statistical concepts and data analysis");
    statsUnit.addProblem(Problem("Mean, Median, Mode", "Measures of central tendency", "Easy"));
    statsUnit.addProblem(Problem("Standard Deviation", "Measuring data spread", "Medium"));
    statsUnit.addProblem(Problem("Probability", "Basic probability calculations", "Medium"));
    statsUnit.addProblem(Problem("Data Visualization", "Creating and interpreting graphs", "Easy"));
    
    units.append(mathUnit);
    units.append(algebraUnit);
    units.append(geometryUnit);
    units.append(statsUnit);
}

QString Model::getProblemStatement(int unitIndex, int problemIndex) const
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        const Unit& unit = units[unitIndex];
        if (problemIndex >= 0 && problemIndex < unit.problems.size()) {
            const Problem& problem = unit.problems[problemIndex];
            if (!problem.problemStatement.isEmpty()) {
                return problem.problemStatement;
            }
            // Fallback to generic problem statement
            return QString("Solve this %1 problem: %2").arg(problem.difficulty.toLower(), problem.name);
        }
    }
    return "Problem not found";
}

QString Model::getTheoryContent(int unitIndex, int problemIndex) const
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        const Unit& unit = units[unitIndex];
        if (problemIndex >= 0 && problemIndex < unit.problems.size()) {
            const Problem& problem = unit.problems[problemIndex];
            if (!problem.theoryContent.isEmpty()) {
                return problem.theoryContent;
            }
            // Fallback to generic theory content
            return QString("Theory for %1\n\n%2\n\nThis is a %3 level topic in %4.")
                   .arg(problem.name, problem.description, problem.difficulty, unit.name);
        }
    }
    return "Theory content not found";
}

QVector<MultipleChoiceOption> Model::getMultipleChoiceOptions(int unitIndex, int problemIndex) const
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        const Unit& unit = units[unitIndex];
        if (problemIndex >= 0 && problemIndex < unit.problems.size()) {
            const Problem& problem = unit.problems[problemIndex];
            if (!problem.choices.isEmpty()) {
                return problem.choices;
            }
        }
    }
    
    // Fallback dummy choices
    return {
        MultipleChoiceOption("A) Option 1", false),
        MultipleChoiceOption("B) Option 2", true),
        MultipleChoiceOption("C) Option 3", false),
        MultipleChoiceOption("D) Option 4", false)
    };
}

int Model::getCorrectChoiceIndex(int unitIndex, int problemIndex) const
{
    QVector<MultipleChoiceOption> choices = getMultipleChoiceOptions(unitIndex, problemIndex);
    for (int i = 0; i < choices.size(); ++i) {
        if (choices[i].isCorrect) {
            return i;
        }
    }
    return 1; // Default to option B if no correct answer found
}

QString Model::unitProblemToString(int unitIndex, int problemIndex) const
{
    if (unitIndex >= 0 && unitIndex < units.size()) {
        const Unit& unit = units[unitIndex];
        if (problemIndex >= 0 && problemIndex < unit.problems.size()) {
            const Problem& problem = unit.problems[problemIndex];
            return QString("Unit: %1, Problem: %2").arg(unit.name, problem.name);
        }
    }
    return "Invalid unit or problem";
}

std::string Model::qStringToStdString(const QString& qstr) const
{
    return qstr.toStdString();
}

QString Model::stdStringToQString(const std::string& str) const
{
    return QString::fromStdString(str);
}

// Current selection methods implementation
std::string Model::getCurrentProblem() const
{
    // Check if we have a valid current selection
    if (currentUnitIndex >= 0 && currentUnitIndex < units.size() &&
        currentProblemIndex >= 0 && currentProblemIndex < units[currentUnitIndex].problems.size()) {
        
        // Get the current problem name and convert to std::string
        const Problem& problem = units[currentUnitIndex].problems[currentProblemIndex];
        return qStringToStdString(problem.name);
    }
    
    // Return empty string if no valid selection
    return "No problem selected";
}

std::string Model::getCurrentDifficulty() const
{
    // Check if we have a valid current selection
    if (currentUnitIndex >= 0 && currentUnitIndex < units.size() &&
        currentProblemIndex >= 0 && currentProblemIndex < units[currentUnitIndex].problems.size()) {
        
        // Get the current problem difficulty and convert to std::string
        const Problem& problem = units[currentUnitIndex].problems[currentProblemIndex];
        return qStringToStdString(problem.difficulty);
    }
    
    // Return empty string if no valid selection
    return "No difficulty selected";
}

void Model::setCurrentSelection(int unitIndex, int problemIndex)
{
    // Validate the indices before setting them
    if (unitIndex >= 0 && unitIndex < units.size() &&
        problemIndex >= 0 && problemIndex < units[unitIndex].problems.size()) {
        
        currentUnitIndex = unitIndex;
        currentProblemIndex = problemIndex;
    } else {
        // Invalid selection - reset to no selection
        currentUnitIndex = -1;
        currentProblemIndex = -1;
    }
}
