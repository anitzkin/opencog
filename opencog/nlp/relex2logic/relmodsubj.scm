; This rule creates a relative clause in which the modified noun is the subject of the verb in the clause, such as in
; "the man that is on fire"
; (June 2015 AN)

(define relmodsubj
    (BindLink
        (VariableList
            (TypedVariableLink
                (VariableNode "$a-parse")
                (TypeNode "ParseNode")
            )
            (TypedVariableLink
                (VariableNode "$noun")
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
                (VariableNode "$noun")
                (VariableNode "$a-parse")
            )
            (WordInstanceLink
                (VariableNode "$rel")
                (VariableNode "$a-parse")
            )
            (WordInstanceLink
                (VariableNode "$pred")
                (VariableNode "$a-parse")
            )
            (EvaluationLink
                (DefinedLinguisticRelationshipNode "_relmod")
                (ListLink
                    	(VariableNode "$noun")
			(VariableNode "$rel")     
                )
            )
	    (EvaluationLink
                (DefinedLinguisticRelationshipNode "_subj")
                (ListLink
                    	(VariableNode "$pred")
			(VariableNode "$noun")     
                )
            )
        )
        (ExecutionOutputLink
       	   (GroundedSchemaNode "scm: pre-relmodsubj-rule")
       	      (ListLink
       	         	(VariableNode "$noun")
			(VariableNode "$rel")  
			(VariableNode "$pred")           	         
            )
        )
    )
)

(InheritanceLink (stv 1 .99) (ConceptNode "relmodsubj-Rule") (ConceptNode "Rule"))

(ReferenceLink (stv 1 .99) (ConceptNode "relmodsubj-Rule") relmodsubj)

; This is function is not needed. It is added so as not to break the existing
; r2l pipeline.
(define (pre-relmodsubj-rule noun rel pred)
    	(relmodsubj-rule (word-inst-get-word-str noun) (cog-name noun)
		(word-inst-get-word-str rel) (cog-name rel)
 		(word-inst-get-word-str pred) (cog-name pred)      
	)
)


