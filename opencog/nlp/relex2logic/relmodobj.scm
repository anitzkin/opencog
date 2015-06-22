; This rule creates a relative clause in which the modified noun is the object of the verb in the clause, such as in
; "the pizza that you ate"
; (June 2015 AN)

(define relmodobj
    (BindLink
        (VariableList
            (TypedVariableLink
                (VariableNode "$a-parse")
                (TypeNode "ParseNode")
            )
            (TypedVariableLink
                (VariableNode "$subj")
                (TypeNode "WordInstanceNode")
            )
            (TypedVariableLink
                (VariableNode "$obj")
                (TypeNode "WordInstanceNode")
            )
            (TypedVariableLink
                (VariableNode "$rel")
                (TypeNode "WordInstanceNode")
            )
	    (TypedVariableLink
                (VariableNode "$pred")
                (TypeNode "WordInstanceNode")
            )
        )
        (AndLink
            (WordInstanceLink
                (VariableNode "$obj")
                (VariableNode "$a-parse")
            )
            (WordInstanceLink
                (VariableNode "$rel")
                (VariableNode "$a-parse")
            )
            (WordInstanceLink
                (VariableNode "$subj")
                (VariableNode "$a-parse")
	    )
            (WordInstanceLink
                (VariableNode "$pred")
                (VariableNode "$a-parse")
	    )
            (EvaluationLink
                (DefinedLinguisticRelationshipNode "_relmod")
                (ListLink
                    	(VariableNode "$obj")
			(VariableNode "$rel")     
                )
            )
	    (EvaluationLink
                (DefinedLinguisticRelationshipNode "_obj")
                (ListLink
                    	(VariableNode "$pred")
			(VariableNode "$obj")     
                )
            )
        )
        (ExecutionOutputLink
       	   (GroundedSchemaNode "scm: pre-relmodobj-rule")
       	      (ListLink
       	         (VariableNode "$obj")
		 (VariableNode "$rel")
		 (VariableNode "$subj")
		 (VariableNode "$pred")        
            )
        )
    )
)

(InheritanceLink (stv 1 .99) (ConceptNode "relmodobj-Rule") (ConceptNode "Rule"))

(ReferenceLink (stv 1 .99) (ConceptNode "relmodobj-Rule") relmodobj)

; This is function is not needed. It is added so as not to break the existing
; r2l pipeline.
(define (pre-relmodobj-rule obj rel subj pred)
    	(relmodobj-rule (word-inst-get-word-str obj) (cog-name obj)
		(word-inst-get-word-str rel) (cog-name rel)  
		(word-inst-get-word-str subj) (cog-name subj)
		(word-inst-get-word-str pred) (cog-name pred)                  
	)
)


